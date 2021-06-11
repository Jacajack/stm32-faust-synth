#include <audio.hpp>
#include <cinttypes>
#include <stdexcept>
#include <aic23b.h>
#include <i2s.h>
#include <i2c.h>

#ifndef AUDIO_BATCH_SIZE
#error  AUDIO_BATCH_SIZE has to be defined!
#endif

#define AUDIO_BUFFER_SIZE (2 * AUDIO_BATCH_SIZE)

//! The audio buffer - contains two audio batches
static uint16_t audio_buffer[AUDIO_BUFFER_SIZE];

//! Pointer to current back part of the audio buffer
static volatile uint16_t *audio_back_buffer = nullptr;

/**
	This flag is set whenever the audio engine is ready to accept new data.
	It is set when DMA transfer finishes and it is cleared when audio_dispatch_*()
	is called.
	
	If the flag is already set when DMA transfer finishes, it is considered a buffer underrun.
*/
volatile bool audio_ready_flag = false;

//! Audio buffer underrun counter
volatile int audio_underrun_counter = 0;

//! Called when the DMA has finished transmitting the second half of the buffer
void HAL_I2S_TxCpltCallback( I2S_HandleTypeDef *h )
{
	if ( audio_ready_flag ) audio_underrun_counter++;
	audio_back_buffer = audio_buffer + AUDIO_BATCH_SIZE;
	audio_ready_flag = true;
}

//! Called when the DMA has finished transmitting the first half of the buffer
void HAL_I2S_TxHalfCpltCallback( I2S_HandleTypeDef *h )
{
	if ( audio_ready_flag ) audio_underrun_counter++;
	audio_back_buffer = audio_buffer;
	audio_ready_flag = true;
}

//! I2S error handler
void HAL_I2S_ErrorCallback( I2S_HandleTypeDef *h )
{
	while ( 1 ); //TODO
}

//! Clamps float data to specified range
static inline float clamp( float x, float min, float max )
{
	if ( x > max ) return max;
	else if ( x < min ) return min;
	else return x;
}

//! Converts a float sample to uint16_t format required by DMA via int16_t in which the codec expects the data. Clamps float data as well.
static inline uint16_t float_to_dma( float x )
{
	return static_cast<uint16_t>( static_cast<int16_t>( 32767 * clamp( x, -1.f, 1.f ) ) );
}

/**
	Returns true if audio engine is ready to accept new data and next call to audio_dispatch_* would be non-blocking
*/
bool audio_is_ready( )
{
	return audio_ready_flag;
}

/**
	Enqueues stereo float data for transmission. If some data has already been dispatched, blocks until current DMA transfer is complete.
	Accepts pointer to float stereo audio buffer of length AUDIO_BATCH_SIZE
*/
void audio_dispatch_stereo( const float *buf )
{
	// Wait for buffer swap if data has already been submitted
	while ( !audio_ready_flag );
	
	for ( int i = 0; i < AUDIO_BATCH_SIZE; i++ )
		audio_back_buffer[i] = float_to_dma( buf[i] );
	audio_ready_flag = false;
}

/**
	Returns length of stereo audio batch
*/
int audio_get_stereo_batch_size( )
{
	return AUDIO_BATCH_SIZE;
}

/**
	Enqueues mono float data for transmission. If some data has already been dispatched, blocks until current DMA transfer is complete.
	Accepts pointer to float mono audio buffer of length AUDIO_BATCH_SIZE/2
*/
void audio_dispatch_mono( const float *buf )
{
	// Wait for buffer swap if data has already been submitted
	while ( !audio_ready_flag );
	
	for ( int i = 0; i < AUDIO_BATCH_SIZE / 2; i++ )
	{
		uint16_t sample = float_to_dma( buf[i] );
		audio_back_buffer[2 * i] = sample;
		audio_back_buffer[2 * i + 1] = sample;
	}
	audio_ready_flag = false;
}

/**
	Returns length of mono audio batch
*/
int audio_get_mono_batch_size( )
{
	return AUDIO_BATCH_SIZE / 2;
}

/**
	Starts audio transmission
*/
void audio_start( )
{
	// Clear the buffer
	for ( int i = 0; i < AUDIO_BUFFER_SIZE; i++ )
		audio_buffer[i] = 0;
	
	// Back buffer is in the second half of the audio buffer at the start and it's ready to accept data
	audio_back_buffer = audio_buffer + AUDIO_BATCH_SIZE;
	audio_ready_flag = true;
	
	// Start the DMA
	HAL_I2S_Transmit_DMA( &audio_i2s, const_cast<uint16_t*>( audio_buffer ), AUDIO_BUFFER_SIZE );
}

/**
	Stops audio transmission
*/
void audio_stop( )
{
	HAL_I2S_DMAStop( &audio_i2s );
}

/**
	Writes registers of TLV320AIC23B codec connected to I2C1.
	PB8 drives codec's CS pin.
*/
static void codec_write_reg( uint8_t addr, uint8_t reg, uint16_t data, int timeout = 100 )
{
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_RESET ); // Set CS low
	aic23b_write_reg( &audio_codec_i2c, addr, reg, data, timeout );
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_8, GPIO_PIN_SET ); // Set CS high
}

/**
	Performs reset and initialization of TLV320AIC23B.
	For now, the sampling rate is 48kHz.
	\todo Improve codec init code
*/
static void codec_init( )
{
	try
	{
		// Reset the codec
		codec_write_reg( CODEC_ADDR, CODEC_REG_RESET, CODEC_RESET_MAGIC );
		HAL_Delay( 100 );
		
		codec_write_reg( CODEC_ADDR, CODEC_REG_DIGITAL_AUDIO_PATH, 0 );
		codec_write_reg( CODEC_ADDR, CODEC_REG_POWER_DOWN, 0 );
		codec_write_reg( CODEC_ADDR, CODEC_REG_DIGITAL_AUDIO_FMT, CODEC_FMT_MASTER | CODEC_FMT_LEN16 | CODEC_FMT_I2S );
		codec_write_reg( CODEC_ADDR, CODEC_REG_SAMPLE_RATE, CODEC_SR_NORMAL ); //48kHz
		codec_write_reg( CODEC_ADDR, CODEC_REG_DIGITAL_IF_ACT, 1 );
		HAL_Delay( 100 );
	}
	catch ( ... )
	{
		throw std::runtime_error( "codec init failed" );
	}
}

/**
	\brief Initializes audio (codec + i2s)
	
	This is actually quite important, because I2S slave mode on this F4 is fucked up - see: https://www.st.com/resource/en/errata_sheet/dm00037591.pdf
	In this implementation, we simply wait for WS to be high and then *enable* (not init) the I2S peripheral.
	Sometimes audio channels are swapped, but that's not a critical issue. At least, there's no more glitched audio.
	To improve that, I2S clock counting algorithm could be used.
	
	The batch_size argument determines DMA batch size in words
*/
void audio_init( )
{
	codec_init( );
	MX_I2S2_Init( ); // TODO replace with custom init (without i2s enable)
	
	// Following sync method could also be replaced with counting 31 clk pulses
	// I think that would reduce number of times when channels are swapped
	__disable_irq( );
	audio_i2s.Instance->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
	while ( HAL_GPIO_ReadPin( GPIOB, GPIO_PIN_12 ) != 0 ); // Wait for WS low
	while ( HAL_GPIO_ReadPin( GPIOB, GPIO_PIN_12 ) == 0 ); // Wait for WS high
	audio_i2s.Instance->I2SCFGR |= SPI_I2SCFGR_I2SE;
	__enable_irq( );
}

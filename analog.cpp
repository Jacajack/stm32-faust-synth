#include <analog.hpp>
#include <adc.h>
#include <gpio.h>
#include <tim.h>

/**
	\todo refactor this entire mostrisity
*/

volatile float mux_inputs[32];
volatile float adc_inputs[4];

volatile int mux_input = 0;
volatile bool analog_read_pending = false;

/**
	Drive 74HC4051 muxes to read selected inputs
*/
static inline void mux_select( unsigned int n )
{
	HAL_GPIO_WritePin( GPIOC, GPIO_PIN_4, static_cast<GPIO_PinState>( n & 1 ) );
	HAL_GPIO_WritePin( GPIOC, GPIO_PIN_5, static_cast<GPIO_PinState>( n & 2 ) );
	HAL_GPIO_WritePin( GPIOB, GPIO_PIN_0, static_cast<GPIO_PinState>( n & 4 ) );
}

/**
	Set input channel on selected ADC
*/
static inline void adc_set_channel( ADC_HandleTypeDef *h, int channel )
{
	static ADC_ChannelConfTypeDef ch;
	ch.Channel = ADC_CHANNEL_0 + channel;
	ch.Rank = 1;
	ch.SamplingTime = ADC_SAMPLETIME_480CYCLES; //144, 480
	HAL_ADC_ConfigChannel( h, &ch );
}

/**
	Function for storing raw ADC data as floats. Implements simple lowpass filtering as well
*/
static inline void adc_value_store( volatile float &dest, uint16_t value )
{
	dest = value * ( 1.f / 4095.f );
}


// Complete conversion counters for ADC1 and ADC3
static volatile int adc1_conv_cnt = 0;
static volatile int adc3_conv_cnt = 0;

// This is called when ADC read has finished
static void analog_read_end( )
{
	if ( ++mux_input == 8 ) mux_input = 0;
	mux_select( mux_input );
	analog_read_pending = false;
}


/**
	Conversion complete callback
	
	For ADC1 and ADC3 this function retriggers a conversion with different input (\todo check ADC scan mode)
*/
void HAL_ADC_ConvCpltCallback( ADC_HandleTypeDef *h )
{
	if ( h == &hadc2 )
	{
		// Code for ADC 2
		return;
	}
	
	// Following section manages ADC read from analog multiplexers
	
	int adc_id = ( h == &hadc3 ); // 0 for ADC1 and 1 for ADC3
	auto &conv_cnt = ( h == &hadc3 ) ? adc3_conv_cnt : adc1_conv_cnt; // Count of conversions performed so far
	
	// Store the read value
	adc_value_store( mux_inputs[mux_input + 16 * adc_id + 8 * conv_cnt], HAL_ADC_GetValue( h ) );
	
	if ( conv_cnt == 0 ) // 1/2 conversions complete
	{
		// Second read from channel 1 for ADC1 and from channel 3 for ADC3
		adc_set_channel( h, adc_id ? 3 : 1 );
		conv_cnt++;
		
		// Start second conversion
		HAL_ADC_Start_IT( h );
	}
	else // 2/2 conversions complete
	{
		conv_cnt++;
		
		// I don't think HAL_ADC_Stop_IT is necessary
	}
	
	if ( adc1_conv_cnt > 1 && adc3_conv_cnt > 1 )
		analog_read_end( );
}

/**
	Triggers process of analog inputs read
*/
void read_analog_inputs( )
{
	if ( analog_read_pending ) return;
	
	adc_set_channel( &hadc1, 0 );
	adc_set_channel( &hadc3, 2 );
	adc1_conv_cnt = adc3_conv_cnt = 0;
	HAL_ADC_Start_IT( &hadc1 );
	HAL_ADC_Start_IT( &hadc3 );
	

	analog_read_pending = true;
}

void analog_read_trigger( )
{
	read_analog_inputs( );
}

void analog_init( float ms )
{
	HAL_TIM_Base_Start_IT( &htim10 );
	htim10.Instance->ARR = 100 * ms - 1;
}

#include <synth.hpp>
#include <string.h>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <type_traits>
#include <cmath>

#include <linear_map.hpp>

#include <com.hpp>

#include <audio.hpp>
#include <analog.hpp>
#include <midi.hpp>
#include <fast_math.hpp>

#include <cstring.hpp>

#include <faust_dsp.hpp>

#include <faust/panel.hpp>
#include <faust/ppg_test2.hpp>
#include <faust/ppg/wavetable.hpp>
#include <faust/ppg/evu10_wavetable.h>

#ifndef DSP_CLASS_NAME
#warning Define DSP_CLASS_NAME!
#define DSP_CLASS_NAME default
#endif

#define MACRO_JOIN_( a, b ) a ## b
#define MACRO_JOIN( a, b ) MACRO_JOIN_( a, b )
#define DSP_CLASS_PREFIX faust_dsp_
#define DSP_CLASS MACRO_JOIN( DSP_CLASS_PREFIX, DSP_CLASS_NAME )
#define DSP_CLASS_HEADER <faust/DSP_CLASS_NAME.hpp>
#include DSP_CLASS_HEADER

/**
	floatbuf[0] = std::tan( floatbuf[0] ); // 182 cycles
	floatbuf[0] = std::sin( floatbuf[0] ); // 123 cycles
	floatbuf[0] = fast_tanf( floatbuf[0] );  // 96-101 cycles (77-82 with fast_rsqrt)
	floatbuf[0] = sqrtf( floatbuf[0] ); // 2  cycles?
	
	\todo fix led code in fault handlers
*/

/**
	Converts a string describing analog input to index in mux_inputs array.
	The input string must be at least 2 characters long, where the first
	character is one of the letters 'abcd'. Following characters must form
	a number between 2 and 10.
*/
static int analog_input_string_to_mux_index( const cstring &s )
{
	if ( s.size( ) < 2 )
		throw std::runtime_error( "Invalid analog input name (length < 2)" );
	
	// Determine connector number
	int conn = std::tolower( s[0] ) - 'a';
	if ( conn < 0 || conn > 3 )
		throw std::runtime_error( "Invalid analog input name (bad connector letter)" );
	
	// Determine pin number
	int pin = std::atoi( s.substr( 1 ) );
	if ( pin < 1 || pin > 10 )
		throw std::runtime_error( "Invalid analog input name (bad pin number)" );
	
	// Pin number -> Multiplexer input
	static const int hash[10] = 
	{
		0, 0, // VCC, GND
		1, 6,
		2, 4,
		0, 7,
		3, 5,
	};
	
	return conn * 8 + hash[pin - 1];
}

std::vector<std::pair<faust_control, volatile float*>> dsp_controls_to_assignments_array( const std::unordered_map<float*, faust_control> &controls )
{
	std::vector<std::pair<faust_control, volatile float*>> assignments;
	
	for ( const auto &[ptr, ctl] : controls )
	{
		// Print the metadata
		if ( ctl.metadata.size( ) )
		{
			comprintf( "Metadata of control parameter %s:\n", ctl.name.c_str( ) );
			for ( const auto &[k,v] : ctl.metadata )
				comprintf( "\t - %s = %s\n", k.c_str( ), v.c_str( ) );
		}

		// Find 'analog' metadata entry
		auto it = ctl.metadata.find( "analog" );
		if ( it == ctl.metadata.end( ) ) continue;
		
		// Assign DSP control to analog input
		int id = analog_input_string_to_mux_index( it->second );
		assignments.emplace_back( ctl, &mux_inputs[id] );
		
		// FIXME
		comprintf( "DSP parameter '%s' is controlled from input %d\n", ctl.name.c_str( ), id );
	}
	
	return assignments;
}

void synth_main( )
{
	// The audio buffer
	size_t buffer_size = audio_get_mono_batch_size( );
	float buffer[buffer_size];

	// The DSP
	faust_dsp dsp( new DSP_CLASS, 48000 );
	
	comprintf( "DSP size: %d\n", sizeof( DSP_CLASS ) );
	
	// Print DSP info
	comprintf( "\n\n" );
	for ( auto [k,v] : dsp.get_metadata( ) )
		comprintf( "%s: %s\n", k.c_str( ), v.c_str( ) );
	comprintf( "dsp controls: %d\n", dsp.get_controls( ).size( ) );
	for ( auto [k,v] : dsp.get_controls( ) )
		comprintf( " - %s\n", v.name.c_str( ) );
	
	// Retreive polyphony information from the dsp
	int polyphony = 1;
	try
	{
		int n = std::atoi( dsp.get_metadata( ).at( "polyphony" ).c_str( ) );
		if ( n ) polyphony = n;
	}
	catch ( const std::out_of_range &ex ) {}

	// Midi interpreter
	polyphonic_midi_controller poly_controller( polyphony );
	midi_interpreter midi( &poly_controller, 0 );
	
	// Get control assignments
	std::vector<std::pair<faust_control, volatile float*>> control_assignments = dsp_controls_to_assignments_array( dsp.get_controls( ) );

	// Start the audio engine
	audio_start( );
	
	float dummy_float;
	float *midi_note_ctl_ptr[polyphony];
	float *midi_gain_ctl_ptr[polyphony];
	float *midi_gate_ctl_ptr[polyphony];

	// Get polyphonic DSP interface
	for ( int i = 0; i < polyphony; i++ )
	{
		char name[64];
		const faust_control *ctl_ptr;
		
		midi_note_ctl_ptr[i] = &dummy_float;
		midi_gain_ctl_ptr[i] = &dummy_float;
		midi_gate_ctl_ptr[i] = &dummy_float;

		std::snprintf( name, 64, "note_%d", i );
		ctl_ptr = dsp.get_control_by_name( name );
		if ( ctl_ptr ) midi_note_ctl_ptr[i] = ctl_ptr->ptr;

		std::snprintf( name, 64, "gain_%d", i );
		ctl_ptr = dsp.get_control_by_name( name );
		if ( ctl_ptr ) midi_gain_ctl_ptr[i] = ctl_ptr->ptr;

		std::snprintf( name, 64, "gate_%d", i );
		ctl_ptr = dsp.get_control_by_name( name );
		if ( ctl_ptr ) midi_gate_ctl_ptr[i] = ctl_ptr->ptr;
	}

	while ( 1 )
	{		
		DWT->CTRL |= 1;
		DWT->CYCCNT = 0;
		
		dsp.compute( buffer_size, {}, {buffer} );
		
		// auto t1 = DWT->CYCCNT;
		
		// Pass note, gain and gate data to the DSP
		for ( int i = 0; i < polyphony; i++ )
		{
			*midi_note_ctl_ptr[i] = poly_controller.get_voice_note( i );
			*midi_gain_ctl_ptr[i] = poly_controller.get_voice_gain( i );
			*midi_gate_ctl_ptr[i] = poly_controller.get_voice_gate( i );
		}
		
		// Update controls from analog inputs
		for ( const auto &[ctl, src] : control_assignments )
			*ctl.ptr = ctl.min + ( ctl.max - ctl.min ) * *src;
		
		// Interpret received MIDI data
		for ( int i = 0; i < midi_data_size; i++ )
			midi.push( midi_data[i] );
		midi_data_size = 0;
		
		// comprintf( "%ld\n", t1 );
		

		audio_dispatch_mono( buffer );

		// Light up both LEDs on underrun
		if ( audio_underrun_counter )
		{
			HAL_GPIO_WritePin( GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_SET );
			audio_underrun_counter--;
		}
		else
		{
			HAL_GPIO_WritePin( GPIOA, GPIO_PIN_11 | GPIO_PIN_12, GPIO_PIN_RESET );
		}	
	}
	

	// 217-218k
}

/**
	Timer interrupt handler
*/
void HAL_TIM_PeriodElapsedCallback( TIM_HandleTypeDef *h )
{
	// TIM10 - Analog input read trigger
	if ( h->Instance == TIM10 )
	{
		analog_read_trigger( );
	}
}

int main( )
{
	// HAL + clock init
	HAL_Init( );
	SystemClock_Config( );
	
	// Peripherals init
	MX_GPIO_Init( );
	MX_DMA_Init( );
	MX_ADC1_Init( );
	MX_ADC2_Init( );
	MX_ADC3_Init( );
	MX_I2C1_Init( );
	MX_USART1_UART_Init( );
	MX_USART3_UART_Init( );
	MX_TIM10_Init( );
	
	// Init audio codec, I2S and sound engine
	audio_init( );
	
	// MIDI init
	midi_init( );
	
	// Init inputs
	analog_init( 5 );
	
	// Start the synthesizer
	try
	{
		synth_main( );
	}
	catch ( const std::exception &ex )
	{
		comprintf( "EXCEPTION: %s\n", ex.what( ) );
	}
	catch ( ... )
	{
		comprintf( "error! damn!\n" );
	}
	
	while ( 1 );
	return 0;
}

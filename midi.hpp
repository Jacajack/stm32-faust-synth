#ifndef MIDI_HPP
#define MIDI_HPP

#include <usart.h>
#include <functional>
#include <deque>

/**
	Base class for all kinds of MIDI controllers. Contains member functions called upon
	differend MIDI events.
*/
class midi_action_handler
{
public:
	virtual void note_on( int key, int velocity ) {}
	virtual void note_off( int key, int velocity ) {}
	virtual void pitchbend( int value ) {}
	virtual void program_change( int program ) {}
	virtual void controller_change( int controller, int value ) {}
	virtual void reset( ) {}
};

/**
	Just a bunch of lambdas handling MIDI events
*/
struct midi_lambdas: public midi_action_handler
{
	virtual void note_on( int key, int velocity )
	{ if ( m_note_on_handler ) m_note_on_handler( key, velocity ); }
	
	virtual void note_off( int key, int velocity )
	{ if ( m_note_off_handler ) m_note_off_handler( key, velocity ); }
	
	virtual void pitchbend( int value )
	{ if ( m_pitchbend_handler ) m_pitchbend_handler( value ); }
	
	virtual void program_change( int program )
	{ if ( m_program_change_handler ) m_program_change_handler( program ); }
	
	virtual void controller_change( int controller, int value )
	{ if ( m_controller_change_handler ) m_controller_change_handler( controller, value ); }
	
	virtual void reset( )
	{ if ( m_reset_handler ) m_reset_handler( ); }
	
	std::function<void(int,int)> m_note_on_handler;
	std::function<void(int,int)> m_note_off_handler;
	std::function<void(int)> m_pitchbend_handler;
	std::function<void(int)> m_program_change_handler;
	std::function<void(int,int)> m_controller_change_handler;
	std::function<void()> m_reset_handler;
};

/**
	Intreprets MIDI commands from data and calls methods in underlying midi_
*/
class midi_interpreter
{
public:
	midi_interpreter( midi_action_handler *handler, uint8_t channel );
	void push( uint8_t b );
	
private:
	int m_data_limit = 0; //!< Expected number of data bytes
	int m_data_count = 0; //!< Number of data bytes received so far 
	uint8_t m_status = 0; //!< Last status byte received
	uint8_t m_channel = 255; //!< Channel number from last status byte
	uint8_t m_data[4]; //!< Buffer for data bytes
	
	midi_action_handler *m_handler; //!< Action handler
	uint8_t m_channel_filter; //!< Current channel
};

/**
	\brief Manages N voices of polyphony based on MIDI Note ON/OFF commands
*/
class polyphony_controller
{
public:
	polyphony_controller( int n );

	void midi_note_on( int key, int velocity );
	void midi_note_off( int key, int velocity );
	void reset( );

	int get_polyphony( ) const
	{
		return m_polyphony;
	}	

	std::function<void(int, int)> get_note_on_lambda( )
	{
		return [&]( int key, int velocity ) { this->midi_note_on( key, velocity ); };
	}

	std::function<void(int, int)> get_note_off_lambda( )
	{
		return [&]( int key, int velocity ) { this->midi_note_off( key, velocity ); };
	}

	const float &get_voice_note( int n ) const
	{
		return m_voice_notes.at( n );
	}

	const float &get_voice_gain( int n ) const
	{
		return m_voice_gains.at( n );
	}

	const float &get_voice_gate( int n ) const
	{
		return m_voice_gates.at( n );
	}

private:
	int m_polyphony; //!< Number of voices

	//! Currently idle voices
	std::deque<int> m_idle;

	//! Queue of busy voices
	std::deque<int> m_busy;

	//! Key-voice mappings - each field corresponds to a MIDI note number
	//! Values are voice IDs. -1 means no mapping
	int m_key_voice_map[128];

	// These will be passed to Faust DSP
	std::vector<float> m_voice_notes;
	std::vector<float> m_voice_gains;
	std::vector<float> m_voice_gates;
};

/**
	\brief Combines polyphony_controller and midi_action_handler
*/
class polyphonic_midi_controller : public midi_action_handler
{
public:
	polyphonic_midi_controller( int n ) :
		m_poly( n )
	{
	}

	virtual void note_on( int key, int velocity )
	{ m_poly.midi_note_on( key, velocity ); }

	virtual void note_off( int key, int velocity )
	{ m_poly.midi_note_off( key, velocity ); }

	virtual void reset( )
	{ m_poly.reset( ); }

	virtual void pitchbend( int value ) 
	{ m_bend = ( value - 8192 ) * ( 1.f / 8192.f ); }
	
	virtual void program_change( int program ) {}
	virtual void controller_change( int controller, int value ) {}



	float get_voice_note( int n ) const
	{
		return m_poly.get_voice_note( n ) + m_bend * m_bend_intensity;
	}

	float get_voice_gain( int n ) const
	{
		return m_poly.get_voice_gain( n );
	}

	float get_voice_gate( int n ) const
	{
		return m_poly.get_voice_gate( n );
	}

private:
	polyphony_controller m_poly;
	float m_bend = 0.f;
	float m_bend_intensity = 2.f;
};

//! Peripheral alias
static UART_HandleTypeDef &midi_uart = huart3;

#define MIDI_BUFFER_SIZE 32

extern volatile int midi_data_size;
extern volatile uint8_t midi_data[MIDI_BUFFER_SIZE];


extern void midi_init( );

#endif

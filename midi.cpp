#include <midi.hpp>
#include <algorithm>

/**
	Buffer for received MIDI commands
*/
volatile uint8_t midi_data[MIDI_BUFFER_SIZE];
volatile uint8_t midi_byte;
volatile int midi_data_size = 0;

/**
	Informs HAL to receive one more byte from MIDI UART
*/
static inline void midi_receive( )
{
	HAL_UART_Receive_IT( &midi_uart, const_cast<uint8_t*>( &midi_byte ), 1 );
}

/**
	General UART Rx complete interrupt. It's here because it's only used for USART3
*/
void HAL_UART_RxCpltCallback( UART_HandleTypeDef *h )
{
	if ( h == &midi_uart )
	{
		// Write the new byte into the buffer
		if ( midi_data_size < MIDI_BUFFER_SIZE )
			midi_data[midi_data_size++] = midi_byte;
		
		// Receive another byte
		midi_receive( );
	}
}

/**
	Initialize MIDI receiver and start receiving MIDI data
*/
void midi_init( )
{
	midi_data_size = 0;
	midi_receive( );
}

/**
	MIDI interpreter constructor - accepts underlying action handler and channel
	\todo make channel filtering more flexible
*/
midi_interpreter::midi_interpreter( midi_action_handler *handler, uint8_t channel ) :
	m_handler( handler ),
	m_channel_filter( channel )
{
}

/**
	Accepts incoming MIDI data byte and calls functions in underlying midi handler class accordingly
*/
void midi_interpreter::push( uint8_t b )
{
	// Abort if there's no action handler
	if ( m_handler == nullptr ) return;
	
	if ( b == 0xff ) m_handler->reset( ); // Reset command
	else if ( b & ( 1 << 7 ) ) // Start of a new MIDI command
	{
		m_status = b & 0x70;
		m_channel = b & 0x0f;
		m_data_count = 0;
		m_data_limit = 0;
		
		// Determine expected data length
		switch ( m_status )
		{
			case 0x00: m_data_limit = 2; break; // Note off
			case 0x10: m_data_limit = 2; break; // Note on
			case 0x30: m_data_limit = 2; break; // Controller change
			case 0x40: m_data_limit = 1; break; // Program change
			case 0x60: m_data_limit = 2; break; // Pitch change
			default: break;
		}
	}
	else if ( m_channel == m_channel_filter ) // Only accept data meant for this device
	{
		// Store incoming data bytes in the buffer
		m_data[m_data_count++] = b;
		
		// If right number of bytes has arrived
		if ( m_data_count >= m_data_limit )
		{
			switch ( m_status )
			{
				// Note off
				case 0x00:
					m_handler->note_off( m_data[0], m_data[1] );
					break;
				
				// Note on
				case 0x10:
					m_handler->note_on( m_data[0], m_data[1] );
					break;
					
				// Controller change
				case 0x30:
					m_handler->controller_change( m_data[0], m_data[1] );
					break;
				
				// Program change
				case 0x40:
					m_handler->program_change( m_data[0] );
					break;
				
				// Pitch change
				case 0x60:
					m_handler->pitchbend( m_data[0] | ( m_data[1] << 7 ) );
					break;
					
				default:
					
					break;
			}
			
			m_data_count = 0;
		}
	}
}



polyphony_controller::polyphony_controller( int n ) :
	m_polyphony( n ),
	m_idle( n ),
	m_voice_notes( n, 0 ),
	m_voice_gains( n, 0 ),
	m_voice_gates( n, 0 )
{
	for ( int i = 0; i < 128; i++ )
		m_key_voice_map[i] = -1;

	for ( int i = 0; i < m_polyphony; i++ )
		m_idle.push_back( i );
}

//! MIDI Note ON event handler
void polyphony_controller::midi_note_on( int key, int velocity )
{
	// Verify key number
	if ( key < 0 || key > 127 ) return;

	// Prevent playing the same note twice
	this->midi_note_off( key, 0 );

	// Get one oscillator
	int id;
	if ( m_idle.size( ) )
	{
		id = m_idle.front( );
		m_idle.pop_front( ); // Pop an idle voice
	}
	else
	{		
		id = m_busy.front( );
		m_busy.pop_front( ); // Pop one from the busy queue
		m_key_voice_map[static_cast<int>( m_voice_notes[id] )] = -1; // Unmap
	}

	// Mark as busy
	m_busy.push_back( id );

	// Register what voice is mapped to certain key
	m_key_voice_map[key] = id;

	// Update parameters passed to Faust
	m_voice_notes[id] = key;
	m_voice_gains[id] = velocity * (1.f / 64.f);
	m_voice_gates[id] = 1.f;		
}

//! MIDI Note OFF event handler
void polyphony_controller::midi_note_off( int key, int velocity )
{
	// Verify key number
	if ( key < 0 || key > 127 ) return;

	// Remove the voice from the key-voice map
	int id = m_key_voice_map[key];
	m_key_voice_map[key] = -1;
	if ( id < 0 ) return;

	// Mark the voice as idle
	auto new_end = std::remove( m_busy.begin( ), m_busy.end( ), id );
	if ( new_end != m_busy.end( ) )
	{
		m_busy.erase( new_end, m_busy.end( ) );
		m_idle.push_back( id );
	}

	// Update parameters passed to Faust
	m_voice_gates[id] = 0.f;
}

//! Turn off all notes
void polyphony_controller::reset( )
{
	while ( !m_busy.empty( ) )
	{
		int id = m_busy.front( );
		m_busy.pop_front( );
		m_idle.push_back( id );
	}
}


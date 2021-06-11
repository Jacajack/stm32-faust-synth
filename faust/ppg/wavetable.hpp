#ifndef PPG_WAVETABLE
#define PPG_WAVETABLE

#include <vector>
#include <stdexcept>


struct wavetable
{
	struct slot
	{
		slot( ) : key( false ), wave_l( -1 ), wave_r( -1 ), dl( 0 ), factor( 0 ) {}
		
		bool key;
		int wave_l, wave_r; // Left and right key wave indices
		float dl;     // Distance from the left key wave
		float factor; // Interpolation factor
	};
	
	slot slots[61];

	void interpolate( );
	
	void get_interpolation_data( float x, int &wave_a, int &wave_b, float &factor );
};

/**
std::ostream &operator<<( std::ostream &s, const wavetable &wt )
{
	for ( auto &w : wt.slots )
	{
		s << w.key << "\t" << w.wave_l << "\t" << w.wave_r << "\t" << w.dl << "\t" << w.factor << std::endl;
	}
	
	return s;
}
*/

void wavetable::interpolate( )
{
	// Pointers to key slots on left and right
	slot *kl = nullptr, *kr = nullptr;
	
	// There has to be a key wave in the last slot
	if ( !slots[60].key )
		throw std::runtime_error( "There's no key wave in the last slot of the wavetable" );
	
	// There has to be a key wave in the first slot
	if ( !slots[0].key )
		throw std::runtime_error( "There's no key wave in the first slot of the wavetable" );
	
	for ( int i = 0; i < 61; i++ )
	{
		// If we encounter a new key wave, update left and right key pointers
		if ( slots[i].key )
		{
			kl = &slots[i];
			
			// Look for the next key slot
			for ( int j = i + 1; j < 61; j++ )
				if ( slots[j].key )
				{
					kr = &slots[j];
					break;
				}
		}
		
		int dt = kr - kl;        // Total distance
		int dl = &slots[i] - kl; // Distance from left key wave
		
		slots[i].wave_l = kl->wave_l;
		slots[i].wave_r = kr->wave_l;
		
		if ( i == 60 ) // Special case for the last slot
		{
			slots[i].factor = 0;
			slots[i].dl = 1;
		}
		else
		{
			slots[i].factor = 1.f / dt;
			slots[i].dl = dl;
		}
	}
}

void wavetable::get_interpolation_data( float x, int &wave_a, int &wave_b, float &factor )
{
	int slot_id = x * 61;
	float slot_rem = 61 * x - slot_id;
	
	wave_a = this->slots[slot_id].wave_l;
	wave_b = this->slots[slot_id].wave_r;
	factor = this->slots[slot_id].factor * ( this->slots[slot_id].dl + slot_rem );
}


// Generate sparse wavetables from data in PPG format
std::vector<wavetable> decode_wavetables( uint8_t *begin, uint8_t *end )
{
	std::vector<wavetable> tables;
	uint8_t *ptr = begin;
	
	while ( ptr < end )
	{
		ptr++; // Ignore that one byte
		
		int wave_id = 0, slot_id = 0;
		wavetable wt;
		
		do
		{
			// Extract wave id and slot number
			if ( ptr == end ) break;
			wave_id = *ptr++;
			if ( ptr == end ) break;
			slot_id = *ptr++;
			
			if ( slot_id >= 61 ) break;
			
			wt.slots[slot_id].wave_l = wt.slots[slot_id].wave_r = wave_id;
			wt.slots[slot_id].key = true;
		}
		while ( slot_id < 0x3c );
		
		
		try
		{
			wt.interpolate( );
		}
		catch ( const std::exception &ex )
		{
			//std::cerr << "error: " << ex.what( ) << std::endl;
			//std::cerr << "skipping..." << std::endl;
			continue;
		}
		
		
		tables.push_back( wt );
	}
	
	return tables;
} 

#endif

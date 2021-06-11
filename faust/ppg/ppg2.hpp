#ifndef FAUST_PPG_2_HPP
#define FAUST_PPG_2_HPP

// PPG Wave waveforms
extern "C" {
#include "evu10.h"
}

#include <vector>
#include "wavetable.hpp"
extern std::vector<wavetable> ppg_tables;

/**
	Provides a way for Faust to read PPG wavetables.
	
	Index is waveform number, phase shall be normalized.
*/
static inline float faust_read_ppg_waveform( int index, float phase )
{
	const uint8_t *const waveform = ppg_evu10 + index * 64;
	
	int sample;
	
	if ( phase < 0.5f )
	{
		int i = 128 * phase;
		sample = waveform[i] - 127;
	}
	else
	{
		int i = 63 - 128 * ( phase - 0.5f );
		sample = -( waveform[i] - 127 );
	}
	
	return sample / 127.f;
}

static inline float faust_read_ppg( int index, float pos, float phase )
{
	int w1, w2;
	float interp;
	ppg_tables[index].get_interpolation_data( pos, w1, w2, interp );
	float s1 = faust_read_ppg_waveform( w1, phase );
	float s2 = faust_read_ppg_waveform( w2, phase );

	return s1 * ( 1.f - interp ) + s2 * interp;
}

#endif

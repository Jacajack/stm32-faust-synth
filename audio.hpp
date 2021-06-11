#ifndef AUDIO_HPP
#define AUDIO_HPP

#include <i2c.h>
#include <i2s.h>

// Peripheral aliases
static I2C_HandleTypeDef &audio_codec_i2c = hi2c1;
static I2S_HandleTypeDef &audio_i2s = hi2s2;

// Audio buffers and pointers
extern volatile int audio_underrun_counter;

// Function prototypes
extern void audio_init( );
extern void audio_start( );
extern void audio_stop( );
extern bool audio_is_ready( );
extern void audio_dispatch_mono( const float *buf );
extern int audio_get_mono_batch_size( );
extern void audio_dispatch_stereo( const float *buf );
extern int audio_get_stereo_batch_size( );

/** \TODO create audio namespace */

#define AUDIO_BATCH_SIZE 512

#endif

#ifndef ANALOG_HPP
#define ANALOG_HPP

extern volatile float mux_inputs[32];
extern volatile float adc_inputs[4];

void read_analog_inputs( );
void analog_init( float ms );
void analog_read_trigger( );

#endif

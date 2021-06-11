import("stdfaust.lib");
import("j.lib");

declare polyphony "4";

// EG
eg( gate ) = en.adsre( A, D, S, R, gate )
with
{
	A = hslider( "A [analog: c5]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	D = hslider( "D [analog: c6]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	S = hslider( "S [analog: c3]", 0.5, 0, 1, 0.001 ) : si.smoo;
	R = hslider( "R [analog: c4]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
};

// filter EG
feg( gate ) = en.adsre( A, D, S, R, gate )
with
{
	A = hslider( "FA [analog: c9]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	D = hslider( "FD [analog: c10]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	S = hslider( "FS [analog: c7]", 0.5, 0, 1, 0.001 ) : si.smoo;
	R = hslider( "FR [analog: c8]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
};


// Oscillators
/*, os.triangle( 50 ), os.square( 120 ), os.square( 222 ), os.square( 44 )*/

osc( note ) = ( polyblep_triangle( f ) + polyblep_saw(f1) + polyblep_saw(f2) ) / 3
// osc( note ) = ( os.triangle( f ) + os.sawtooth(f1) + os.sawtooth(f2) ) / 3
with
{
	oct1 = hslider( "osc1oct [analog: d10]", 0, -1.5, 1.5, 1 ) : int : _ * 12;
	oct2 = hslider( "osc2oct  [analog: d9]", 0, -1.5, 1.5, 1 ) : int : _ * 12;
  	note1 = note + ( hslider( "osc1tune [analog: d7]", 0, -12, 12, 0.001 ) : si.smoo ) + oct1;
	note2 = note + ( hslider( "osc2tune [analog: d8]", 0, -12, 12, 0.001 ) : si.smoo ) + oct2;
	tri_enabled = hslider( "trienabled [analog: d4]", 0, 0, 1.5, 0.001 ) : int;
	f = note : mid2hz;
	f1 = note1 : mid2hz;
	f2 = note2 : mid2hz;
};

// Filter Moog
lpf_moog( envelope ) = ve.moog_vcf_2b( resonance, cutoff )
with
{
	fc_knob = hslider( "fc [analog: d3]", 0.5, 0, 1, 0.001 ) : si.smoo;
	envelope_int = hslider( "fc_env_int [analog: d6]", 0, -1, 1, 0.001 ) : si.smoo;
	resonance = hslider( "reso [analog: d5]", 0.5, 0, 1, 0.001 ) : si.smoo;
  
	fc_env = envelope_int * envelope;
	cutoff =  ( fc_knob + fc_env ) : min( 1 ) : max( 0 ) : lin2exp( 20, 20000 );
};

// Filter Korg
lpf( envelope ) = ve.korg35LPF( cutoff, resonance )
with
{
	fc_knob = hslider( "fc [analog: d3]", 0.5, 0, 1, 0.001 ) : si.smoo;
	envelope_int = hslider( "fc_env_int [analog: d6]", 0, -1, 1, 0.001 ) : si.smoo;
	resonance = hslider( "reso [analog: d5]", 0.5, 0, 10, 0.001 ) : si.smoo;
  
	fc_env = envelope_int * envelope;
	cutoff =  ( fc_knob + fc_env ) : min( 1 ) : max( 0 );
};

voice( note, gate ) = oscillator : filter * envelope
with
{
	oscillator = osc( note );
	envelope = eg( gate );
	filter = lpf( feg( gate ) );
};

gate_0 = button( "gate_0" );
gate_1 = button( "gate_1" );
gate_2 = button( "gate_2" );
gate_3 = button( "gate_3" );

note_0 = hslider( "note_0", 0, 0, 127, 1 );
note_1 = hslider( "note_1", 0, 0, 127, 1 );
note_2 = hslider( "note_2", 0, 0, 127, 1 );
note_3 = hslider( "note_3", 0, 0, 127, 1 );

process = 0.25 * ( voice( note_0, gate_0 ) + voice( note_1, gate_1 ) + voice( note_2, gate_2 ) + voice( note_3, gate_3 ) );
// process = ( voice( note_0, gate_0 ) + voice( note_1, gate_1 ) + voice( note_2, gate_2 )  ) / 3;
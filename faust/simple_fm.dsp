import("stdfaust.lib");

declare polyphony "4";

osc( f ) = sin( os.phasor( 2 * ma.PI, f ) );
lin2exp( mi, ma, x ) = exp( log( ma ) * x + log( mi ) * ( 1 - x ) );
mid2hz( k ) = 440.0 * exp( ( k - 69 ) * log( pow( 2, 1 / 12 ) ) ); 

// Envelope generator for operator 0
eg0( g ) = en.adsre( a, d, s, r, g )
with 
{
	a = hslider( "a0 [analog: c5]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	d = hslider( "d0 [analog: c6]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	s = hslider( "s0 [analog: c3]", 0.5, 0, 1, 0.001 ) : si.smoo;
	r = hslider( "r0 [analog: c4]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
};

// Envelope generator for operator 1
eg1( g ) = en.adsre( a, d, s, r, g )
with 
{
	a = hslider( "a1 [analog: c9]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	d = hslider( "d1 [analog: c10]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	s = hslider( "s1 [analog: c7]", 0.5, 0, 1, 0.001 ) : si.smoo;
	r = hslider( "r1 [analog: c8]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
};

oct = hslider( "osc1oct [analog: d10]", 0, -1.5, 1.5, 1 ) : int : _ * 12;
kA = hslider( "kA [analog: d5]", 0.5, 0, 4, 0.0001 ) : si.smoo;
kf = hslider( "kf [analog: d3]", 0, 0, 4, 0.001 ) : si.smoo;

gate_0 = button( "gate_0" );
gate_1 = button( "gate_1" );
gate_2 = button( "gate_2" );
gate_3 = button( "gate_3" );

note_0 = hslider( "note_0", 0, 0, 127, 1 );
note_1 = hslider( "note_1", 0, 0, 127, 1 );
note_2 = hslider( "note_2", 0, 0, 127, 1 );
note_3 = hslider( "note_3", 0, 0, 127, 1 );

// Simple FM synthesis (2 operators)
voice( note, gate ) = op0
with
{
	f0 = note + oct : mid2hz;
	f1 = f0 * kf;
	A0 = 1;
	A1 = f0 * kA;	
	op1 = A1 * eg1( gate ) * osc( f1 );
	op0 = A0 * eg0( gate ) * osc( f0 + op1 );
};

process = ( voice( note_0, gate_0 ) + voice( note_1, gate_1 ) + voice( note_2, gate_2 ) + voice( note_3, gate_3 ) ) / 4;
	
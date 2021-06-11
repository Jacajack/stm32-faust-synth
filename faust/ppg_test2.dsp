import("stdfaust.lib");

mid2hz( k ) = 440.0 * exp( ( k - 69 ) * log( pow( 2, 1 / 12 ) ) ); 
lin2exp( mi, ma, x ) = exp( log( ma ) * x + log( mi ) * ( 1 - x ) );
ppg_waveform = ffunction( float faust_read_ppg(int, float, float), "ppg/ppg2.hpp", "" );

gate = button( "gate" );
f = hslider( "f", 55, 20, 220, 0.001 ) : mid2hz;


sel = ( hslider( "sel[analog: d8]", 8, -0.5, 30.5, 1 ) : int );
interp = hslider( "[analog: d7]", 0.5,  0, 1, 0.001 );


filter = ve.moog_vcf_2b( resonance, cutoff )
with
{
	fc = hslider( "fc [analog: d3]", 0.5, 0, 1, 0.001 ) : si.smoo : lin2exp( 20, 20000 );
	resonance = hslider( "reso [analog: d5]", 0.5, 0, 1, 0.001 ) : si.smoo : lin2exp( 0.0001, 1.0 );
	cutoff =  fc : min( 20000 ) : max( 20 );
};

envelope = en.adsre( A, D, S, R, gate )
with
{
	gate = button( "gate" );
	A = hslider( "A [analog: c5]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	D = hslider( "D [analog: c6]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
	S = hslider( "S [analog: c3]", 0.5, 0, 1, 0.001 ) : si.smoo;
	R = hslider( "R [analog: c4]", 0.5, 0, 1, 0.001 ) : lin2exp( 0.01, 4 ) : si.smoo;
};

process = ppg_waveform( sel, interp, os.phasor( 1, f / 2 ) ) : filter * envelope;

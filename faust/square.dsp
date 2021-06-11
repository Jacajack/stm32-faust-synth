import("stdfaust.lib");
//process = os.square( hslider("note_freq", 0, 1, 50, 1e-3 ) ) * en.adsr( 1e-3, 0.2, 0.7, 0.5, button( "gate" ) );

process = (os.square( 220 ) + os.square( 110.5 ))/2;
			

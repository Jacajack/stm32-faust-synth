#ifndef FAUST_FAST_MATH_HPP
#define FAUST_FAST_MATH_HPP

#include <cmath>
#include <cstdio>

#include <fast_math_data.hpp>

// Make sure that PI is defined as a float
#undef  M_PI
#define M_PI 3.14159265358979323846f

static inline float fast_fabsf(float x)
{
	return fabsf(x);
}

//! Fast float floor()
static inline float fast_floorf( float x )
{
	int i = x;
    return ( x >= 0 ) ? i : i - 1;
	// return static_cast<int>( x ) - ( x < static_cast<int>( x ) );
}

//! Fast float ceil()
static inline float fast_ceilf( float x )
{
	return static_cast<int>( x ) + ( x > static_cast<int>( x ) );
}

//! Fast fmodf()
static inline float fast_fmodf( float x, float y )
{
	return x - y * fast_floorf( x / y );
}

/**
	log2(x) approximated with Taylor series. It's not perfect
	and relies on substitution I was to lazy to eliminate by hand,
	so it could be better.
*/
static inline float fast_log2f_taylor_manual( float x )
{
	static const float C = std::log2( 1.5f );
	static const float C2 = 1.f / ( 60.f * std::log( 2 ) );
	float t = x / 1.5f - 1.f;
	//return C + C2 * ((((12*t-15)*t+20)*t-30)*t+60)*t;
	return fmaf(C2, t*fmaf(t, fmaf(t, fmaf(t, fmaf(t, 12, -15), 20), -30), 60), C);
}


/**
	Fast logarithm approximation (around 1.5) with Taylor series.
	
	sage: ln(x).taylor(x, 1.5, 5).horner(x)
	(((x*(0.02633744855967078*x - 0.24691358024691357) + 0.9876543209876543)*x - 2.2222222222222223)*x + 3.333333333333333)*x - 1.8778682252251688
*/
static inline float fast_logf_taylor( float x )
{
	return fmaf( fmaf( fmaf( fmaf( fmaf(0.02633744855967078f, x, -0.24691358024691357f), x, 0.9876543209876543f), x, -2.2222222222222223f), x, 3.333333333333333f), x, -1.8778682252251688f);
};

static inline float fast_log2f_taylor( float x )
{
	return fast_logf_taylor( x ) * 1.44269504089f;
};

/**
	\brief Computes log2(x) based on Taylor series and floating point
	bitwise manipulation.
*/
static inline float fast_log2f( float x )
{
	union { float xf; uint32_t xi; };

	// Extract exponent and mantissa
	xf = x;
	int ei = ( ( xi >> 23 ) & 0xff ) - 127; // Exponent
	int mi = xi & 0x7fffff;                 // Mantissa
	
	// Real mantissa value
	float mf = 1.f + mi * ( 1.f / 0x7fffff );
	
	// Denormal numbers (optional)
	// if ( ei == -127 ) mf = mi * ( 1.f / 0x7fffff );

	return ei + fast_log2f_taylor( mf );
}

/**
	Fast natural logarithm
*/
static inline float fast_logf( float x )
{
	static const float c = std::log( 2 );
	return c * fast_log2f( x );
}

/**
	Exponential function approximation with Taylor series.
	Accurate in [-0.5; 0.5]. It's faster when using MAD instructions.
*/
static inline float taylor_exp5( float x )
{
	return fmaf( x, fmaf( x, fmaf( x, fmaf( x, x + 5.f, 20.f ), 60.f ), 120.f ), 120.f ) * 0.0083333333f;
	// return (120+x*(120+x*(60+x*(20+x*(5+x)))))*0.0083333333f;
}

/**
	Exponential function approximated with Taylor series.
	Accurate in [0;1]
*/
static inline float taylor_exp5_2( float x )
{
	float t = x - 0.5f;
	return fmaf( t, fmaf( t, fmaf( t, fmaf( t, t + 5, 20 ), 60 ), 120 ), 120 ) * 0.0137393439225f;
}

/**
	Exponential function approximated with Taylor series. Generated with sage.
	Accurate in [0;1]
*/
static inline float taylor_exp5_sage( float x )
{
	//return (((x*(0.01373934392250107*x+0.03434835980625267)+0.1717417990312634)*x+0.4980512171906636)*x+1.0003959793571089)*x+0.9999666248595308;
	return fmaf(fmaf(fmaf(fmaf(fmaf(0.01373934392250107f, x, 0.03434835980625267f), x, 0.1717417990312634f), x, 0.4980512171906636f), x, 1.0003959793571089f), x, 0.9999666248595308f);
}

/**
	Fast exponential function approximation - combines lookup table and taylor_exp()
	\todo negative arguments LUT
*/
static inline float fast_expf_lut_taylor( float f )
{
	static const auto taylor_function = taylor_exp5_2;
	static const auto exp_lut  = exp_lut_256;
	static const int  lut_zero = 128;
	
	
	int i = fast_floorf( f ); // Integer part
	float r = f - i;          // Remainder [0;1)

	return exp_lut[( lut_zero + i ) & 0xff] * taylor_function( r );
}

/**
	Wrapper for fast exponential function
*/
static inline float fast_expf( float x )
{
	return fast_expf_lut_taylor( x );
}

//! Fast pow wrapper
static inline float fast_powf( float x, float y )
{
	// return fast_powf_bad( x, y );
	// return std::pow( x, y );
	return fast_expf( y * fast_logf( x ) );
}

/**
	Just a wrapper for std::sqrt, because it's supported by the FPU
*/
static inline float fast_sqrtf( float x )
{
	return sqrtf( x );
}

/**
	Relatively fast approximate tangent calculation based on numerical approximations and a lookup table.
*/
static inline float fast_tanf_hybrid( float x )
{
	x = x - M_PI * fast_floorf( x / M_PI ); // Modulo pi
	if ( x == M_PI / 2.f ) return NAN; // Handle  tan(pi/2 + k*pi)

	const int N = TAN_LUT_SIZE;
	
	// LUT index and remainder
	int index = x * ( N / M_PI );
	float r = x - index * ( M_PI / N );
	
	// T is tangent from LUT, and t is small angle tangent approximation
	float t = r / fast_sqrtf( 1.f - r * r );
	float T = tan_lut[index];
	
	// Tangent of sum
	return ( t + T ) / ( 1.f - T * t ); 
}

/**
	Fast tangent approximation based entirely on lookup table and linear interpolation.
	
	Faster than fast_tanf_hybrid()
	
	\todo add LUT with 1024 samples and handle index==1023 case
	\todo make sure LUT is stored in flash - put it in separate c module
*/
static inline float fast_tanf_lookup( float x )
{
	x = x - M_PI * fast_floorf( x / M_PI ); // Modulo pi
	if ( x == M_PI / 2.f ) return NAN; // Handle  tan(pi/2 + k*pi)
	
	const int N = TAN_LUT_SIZE;
	
	// Lookup index and interpolation coefficient
	int index = x * ( N / M_PI );
	float r = x * ( N / M_PI ) - index;
	
	// Do the lookup
	float t = tan_lut[index];
	float T = tan_lut[index + 1]; // FIXME
	
	// Linear interpolation
	return fmaf( t, 1.f - r, T * r );
}

/**
	Fast tangent based on Pade approximation. Generated by sage.
	
	Faster than fast_tanf_lookup()
	
	sage: tan(x).taylor(x, 0, 12).power_series(QQ).pade(3, 3)
	(1/6*x^3 - 5/2*x)/(x^2 - 5/2)
*/
static inline float fast_tanf_pade( float x )
{
	x = x - M_PI * fast_floorf( x / M_PI ); // Modulo pi
	return x * ( (1.f/6.f)*x*x - (5.f/2.f) ) / ( x * x - (5.f/2.f) );
}

/**
	A wrapper for fast tangent
*/
static inline float fast_tanf( float x )
{
	return fast_tanf_pade( x );
}

/**
	Sine approximation accurate in [0; pi/2].
	Based on Chebyshev approximation taken from here: https://stackoverflow.com/a/394512/4019966
*/
static inline float fast_sinf_chebyshev( float x )
{
	return x * ( 0.9996949f + x * x * ( -0.1656700f + 0.0075134f * x * x ) ); //! \todo use fmaf()
}

/**
	Fast sine approximation.
*/
static inline float fast_sinf( float x )
{
	x = fast_fmodf( x, 2 * M_PI );

	// Flip the half above M_PI
	float sign = 1.f;
	if ( x > M_PI ) 
	{
		x -= M_PI;
		sign = -1.f;
	}

	if ( x < M_PI / 2 )
		return sign * fast_sinf_chebyshev( x );
	else
		return sign * fast_sinf_chebyshev( M_PI - x );
}

#endif

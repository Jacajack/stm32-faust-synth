#ifndef COM_HPP
#define COM_HPP

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <usart.h>

static inline int comstr( const char *s )
{
	auto len = strlen( s );
	HAL_UART_Transmit( &huart1, reinterpret_cast<uint8_t*>( const_cast<char*>( s ) ), len, 100 );
	return len;
}

/**
	\todo Use DMA!!!!
*/
static inline int comprintf( const char *format, ... ) __attribute__((format(printf, 1, 2)));
static inline int comprintf( const char *format, ... ) 
{
	static char buf[256];
	
	va_list ap;
	va_start( ap, format );
	int len = vsnprintf( buf, sizeof buf, format, ap );
	va_end( ap );
	comstr( buf );
	return len;
}

#endif
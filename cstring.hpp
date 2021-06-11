#ifndef CSTRING_HPP
#define CSTRING_HPP

#include <string>
#include <cstring>
#include <stdexcept>

/**
	A lightweight wrapper for C strings.

	\note This class does not carry the string data. It's just a fancy pointer wrapper.

	\todo verify handling of null/empty strings
*/
class cstring
{
public:
	cstring( const char *s ) :
		m_str( s ),
		m_length( 0 )
	{
		if ( s != nullptr )
			m_length = std::strlen( s );
	}
	
	cstring( const std::string &s ) :
		m_str( s.c_str( ) ),
		m_length( s.length( ) )
	{
	}
	
	cstring( ) :
		m_str( nullptr ),
		m_length( 0 )
	{
	}
	
	//! Return pointer to the begining
	const char *cbegin( ) const
	{
		return m_str;
	}
	
	//! Return end pointer
	const char *cend( ) const
	{
		return m_str + m_length;
	}
	
	//! Finds a substring
	const char *find( const char *what )
	{
		auto it = std::strstr( m_str, what );
		if ( it == nullptr ) return cend( );
		else return it;
	}
	
	//! Returns a substring (from nth character onwards)
	const char *substr( size_t pos ) const
	{
		if ( pos >= m_length )
			throw std::out_of_range( "cstring::substr() - access outside of contained C string" );
		return m_str + pos;
	}
	
	//! Implicit cast to const char*
	explicit operator const char*( ) const
	{
		return m_str;
	}
	
	//! Returns the C string
	const char *c_str( ) const
	{
		return m_str;
	}
	
	//! Returns length of the string
	size_t length( ) const
	{
		return m_length;
	}
	
	//! Checks if the string contains a substring
	bool contains( const cstring &s ) const
	{
		return std::strstr( this->c_str( ), s.c_str( ) ) != nullptr;
	}

	//! Returns length of the string
	size_t size( ) const
	{
		return m_length;
	}
	
	bool empty( ) const
	{
		return size( ) == 0;
	}
	
	//! Checks if two strings are equal
	bool operator==( const cstring &rhs ) const
	{
		if ( this->size( ) != rhs.size( ) ) return false;
		else if ( empty( ) && rhs.empty( ) ) return true;
		else return !std::strcmp( m_str, rhs.m_str );
	}
	
	//! Compares strings
	bool operator<( const cstring &rhs ) const
	{
		if ( empty( ) ) return -1;
		else return std::strcmp( m_str, rhs.m_str );
	}
	
	//! Returns string characters
	const char &operator[]( int index ) const noexcept
	{
		return m_str[index];
	}
	
	//! Throwing version of operator[]
	const char &at( size_t index ) const
	{
		if ( index >= m_length )
			return m_str[index];
		else
			throw std::out_of_range( "cstring::at() - access outside contained C string" );
	}

private:
	const char *m_str;
	size_t m_length;
};

/**
	A std::hash specialization for cstring class
*/
template<>
struct std::hash<cstring>
{
	// djb2 string hashing algorithm
	std::size_t operator()( const cstring &cs ) const
	{
		std::size_t h = 5381;
		const char *str = cs.c_str( );
		int c;

		while ( ( c = *str++ ) )
			h = ((h << 5) + h) + c; /* h * 33 + c */

		return h;
	}
};

#endif

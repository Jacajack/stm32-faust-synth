#ifndef LINEAR_MAP_HPP
#define LINEAR_MAP_HPP

#include <vector>
#include <utility>
#include <stdexcept>

/**
	Simple std::map-like class based on linear lookup.
	This is by no means effective, but uses less memory and C++ fancy stuff.
*/
template <typename Tk, typename Tv>
class linear_map
{
public:
	using value_type = std::pair<Tk, Tv>;
	using vector_type = std::vector<value_type>;
	using iterator = typename vector_type::iterator;
	using const_iterator = typename vector_type::const_iterator;

	/**
		Returns reference to value associated with given key.
		If the key doesn't exist, throws.
	*/
	const Tv &at( const Tk &key ) const
	{
		for ( const auto &[k,v] : m_pairs )
			if ( k == key )
				return v;
		throw std::out_of_range( "key not found in linear_map" );
	}
	
	Tv &at( const Tk &key )
	{
		for ( auto &[k,v] : m_pairs )
			if ( k == key )
				return v;
		throw std::out_of_range( "key not found in linear_map" );
	}
	
	/**
		Returns reference to value associated with given key.
		If the entry doesn't exits, creates a new pair in the map.
	*/
	Tv &operator[]( const Tk &key )
	{
		try
		{
			return this->at( key );
		}
		catch ( const std::out_of_range &ex )
		{
			m_pairs.emplace_back( key, Tv{} );
			return m_pairs.back( ).second;
		}
	}

	/**
		Checks if the map contains given key
	*/
	bool contains( const Tk &key ) const
	{
		try
		{
			this->at( key );
			return true;
		}
		catch ( const std::out_of_range &ex )
		{
			return false;
		}
	}

	const_iterator cbegin( ) const
	{ return m_pairs.cbegin( ); }

	iterator begin( )
	{ return m_pairs.begin( ); }

	const_iterator cend( ) const
	{ return m_pairs.cend( ); }

	iterator end( )
	{ return m_pairs.end( ); }
	
private:
	vector_type m_pairs;
};

#endif

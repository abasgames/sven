#pragma once
/* author: guwi */
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <any>
#include <fstream>
#include <vector>

namespace hash {
	constexpr const uint32_t fnv1_basis = 0x811C9DC5;
	constexpr const uint32_t fnv1_prime = 0x1000193;

	// compile time
	constexpr uint32_t hash32( const char* string, const uint32_t basis = fnv1_basis ) {
		return ( string[ 0 ] == '\0' ) ? basis : hash32( &string[ 1 ], ( basis ^ string[ 0 ] ) * fnv1_prime );
	};

	// runtime
	struct fnv1a_internal {
		static uint32_t hash( const char* string ) {
			uint32_t m_hash = fnv1_basis;
			for ( std::size_t i = 0; i < strlen( string ); i++ ) {
				m_hash ^= string[ i ];
				m_hash *= fnv1_prime;
			};

			return m_hash;
		};
	};
	using fnv1a = fnv1a_internal;
};

#define HASH_32( x ) hash::fnv1a::hash( x )
#define HASH_32_C( x ) hash::hash32( x )

struct var_t {
	var_t( uint32_t hashed, uint32_t hashtype, std::any&& v ) : m_hash{ hashed }, m_hash_type{ hashtype }, m_value{ std::move( v ) } { };
	~var_t( ) = default;
	template< typename tty = var_t > tty& as( ) { return *static_cast< tty* >( std::any_cast< tty >( &m_value ) ); };
	template< typename tty = var_t > void set( tty val ) { m_value.emplace< tty >( val ); };

	std::uint32_t m_hash = 0x000000;
	std::uint32_t m_hash_type = 0x000000;
	std::any m_value{ };

	friend std::ostream& operator<<( std::ostream&, const var_t& );
	friend std::istream& operator>>( std::istream&, var_t& );
};

namespace cfg {
	inline std::vector< var_t > _variables{ };

	template< typename tty >
	inline tty& get( const std::uint32_t idx ) {
		return _variables.at( idx ).as< tty >( );
	};

	template< typename tty >
	inline constexpr void set( const std::uint32_t idx, const tty& value ) noexcept {
		_variables.at( idx ).set< tty >( value );
	};

	inline var_t& var( const std::uint32_t idx ) {
		return _variables.at( idx );
	};

	template< typename tty >
	inline constexpr std::uint32_t add( const std::uint32_t hash, const std::uint32_t hashtype, const tty value ) noexcept  {
		_variables.push_back( { hash, hashtype, std::make_any< tty >( value ) } );
		return _variables.size( ) - 1;
	};

	inline std::uint32_t get_index_hash( const std::uint32_t hash ) {
		int m_index = 0;
		for ( auto it : _variables ) {
			if ( it.m_hash == hash )
				return m_index;
			m_index++;
		};

		return 0x000000;
	};

	inline void save_config( const char* file ) noexcept {
		std::ofstream sz_config( file );
		for ( auto& of : _variables )
			sz_config << of;
		sz_config.close( );
	};

	inline void load_config( const char* file ) noexcept {
		std::ifstream sz_config( file );
		for ( auto& of : _variables )
			sz_config >> of;
		sz_config.close( );
	};

	inline constexpr std::vector< var_t > get_variables( void ) noexcept { return _variables; };
};

inline std::ostream& operator<<( std::ostream& os, const var_t& acc )
{
	os << acc.m_hash << " " << acc.m_hash_type << " ";
	switch ( acc.m_hash_type ) {
	case HASH_32_C( "bool" ): os << std::any_cast< bool >( acc.m_value ) << std::endl; break;
	case HASH_32_C( "float" ): os << std::any_cast< float >( acc.m_value ) << std::endl; break;
	case HASH_32_C( "int" ): os << std::any_cast< int >( acc.m_value ) << std::endl; break;
	case HASH_32_C( "double" ): os << std::any_cast< double >( acc.m_value ) << std::endl; break;
	};
	return os;
};

inline std::istream& operator>>( std::istream& is, var_t& acc ) {

	uint32_t hash, hashtype;
	is >> hash >> hashtype; //acc.m_hash >> acc.m_hash_type;

	if ( hash == acc.m_hash ) {
		switch ( acc.m_hash_type ) {
		case HASH_32_C( "bool" ): { bool temp; is >> temp; acc.set< bool >( temp ); }; break;
		case HASH_32_C( "float" ): { float temp; is >> temp; acc.set< float >( temp ); }; break;
		case HASH_32_C( "int" ): { int temp; is >> temp; acc.set< int >( temp ); }; break;
		case HASH_32_C( "double" ): { double temp; is >> temp; acc.set< double >( temp ); }; break;
		default: std::printf( "unsupported mode.\n" ); break;
		};
	}
	return is;
};

#define ADD_VAR( name, type, value ) \
const std::uint32_t name = cfg::add< type >( HASH_32( #name ), HASH_32( #type ), value )
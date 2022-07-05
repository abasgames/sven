#pragma once
#include <utility>
#include <array>

namespace hash {
	template< std::size_t n, int k >
	class c_xor_str {
	public:
		template< std::size_t... is >
		constexpr inline c_xor_str( const char* string, std::index_sequence< is... > ) : _key{ k }, encrypted_data{ enc( string[ is ] )... } { };

		constexpr inline auto decrypt( void ) {
			for ( std::size_t i = 0; i != n; ++i )
				encrypted_data[ i ] = dec( encrypted_data[ i ] );

			encrypted_data[ n ] = '\0';

			return encrypted_data.data( );
		};
	private:
		constexpr char enc( char c ) const {
			return c ^ _key;
		};

		char dec( char c ) const {
			return c ^ _key;
		};

	private:
		const char _key;
		std::array< char, n + 1 > encrypted_data;
	};
};

#define xorstr( str ) hash::c_xor_str< sizeof( str ) - 1, __COUNTER__ >( str, std::make_index_sequence< sizeof( str ) - 1 >( ) ).decrypt( )
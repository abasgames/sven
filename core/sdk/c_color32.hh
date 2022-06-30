#pragma once
#include <cstdint>
#include <cmath>

namespace sdk {
	class c_color32 {
	private:
		std::uint8_t colors[ 4 ];

		void set_color( int r, int g, int b, int a ) {
			colors[ 0 ] = static_cast< std::uint8_t >( r );
			colors[ 1 ] = static_cast< std::uint8_t >( g );
			colors[ 2 ] = static_cast< std::uint8_t >( b );
			colors[ 3 ] = static_cast< std::uint8_t >( a );
		};
	public:
		c_color32( ) { *( ( int* )this ) = 0; };
		c_color32( int r, int g, int b, int a = 255 ) { set_color( r, g, b, a ); };

		inline int r( ) const { return colors[ 0 ]; };
		inline int g( ) const { return colors[ 1 ]; };
		inline int b( ) const { return colors[ 2 ]; };
		inline int a( ) const { return colors[ 3 ]; };
		inline uint32_t hex_abgr( ) const { return ( ( colors[ 3 ] << 24 | colors[ 2 ] << 16 | colors[ 1 ] << 8 | colors[ 0 ] ) ); }; // sven coop specific.
		inline std::uint8_t operator[]( int index ) { return colors[ index ]; };
		inline std::uint8_t* get_scheme( ) { return colors; };
		static c_color32 hsb_to_rgb( float hue, float saturation, float brightness ) {
			int r = 0, g = 0, b = 0;
			if ( saturation == 0 ) {
				r = g = b = ( int )( brightness * 255.0f + 0.5f );
			}
			else {
				float h = ( hue - floorf( hue ) ) * 6.0f;
				float f = h - floorf( h );
				float p = brightness * ( 1.0f - saturation );
				float q = brightness * ( 1.0f - saturation * f );
				float t = brightness * ( 1.0f - ( saturation * ( 1.0f - f ) ) );
				switch ( ( int )h ) {
				case 0:
					r = ( int )( brightness * 255.0f + 0.5f );
					g = ( int )( t * 255.0f + 0.5f );
					b = ( int )( p * 255.0f + 0.5f );
					break;
				case 1:
					r = ( int )( q * 255.0f + 0.5f );
					g = ( int )( brightness * 255.0f + 0.5f );
					b = ( int )( p * 255.0f + 0.5f );
					break;
				case 2:
					r = ( int )( p * 255.0f + 0.5f );
					g = ( int )( brightness * 255.0f + 0.5f );
					b = ( int )( t * 255.0f + 0.5f );
					break;
				case 3:
					r = ( int )( p * 255.0f + 0.5f );
					g = ( int )( q * 255.0f + 0.5f );
					b = ( int )( brightness * 255.0f + 0.5f );
					break;
				case 4:
					r = ( int )( t * 255.0f + 0.5f );
					g = ( int )( p * 255.0f + 0.5f );
					b = ( int )( brightness * 255.0f + 0.5f );
					break;
				case 5:
					r = ( int )( brightness * 255.0f + 0.5f );
					g = ( int )( p * 255.0f + 0.5f );
					b = ( int )( q * 255.0f + 0.5f );
					break;
				};
			};
			return c_color32( r, g, b, 255 );
		};
	};
};
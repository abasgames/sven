#pragma once
#include <cmath>

namespace sdk {
	class c_vector {
	public:
		float x, y, z;

		c_vector( ) : x( 0.0f ), y( 0.0f ), z( 0.0f ) { };
		c_vector( float xyz ) : x( xyz ), y( xyz ), z( xyz ) { };
		c_vector( float x, float y, float z = 0.0f ) : x( x ), y( y ), z( z ) { };

		inline float length( ) const {
			return x * x + y * y + z * z;
		};

		inline float length2d( ) const {
			return x * x + y * y;
		};

		inline float length_sqrtf( ) const {
			return sqrtf( length( ) );
		};

		inline float length2d_sqrtf( ) const {
			return sqrtf( length2d( ) );
		};

		inline float dot_product( const c_vector& a, const c_vector& b ) {
			return ( a.x * b.x + a.y * b.y + a.z * b.z );
		};

		inline float dot( const c_vector& other ) {
			return dot_product( *this, other );
		};

		inline constexpr bool is_zero( ) const {
			return ( x == 0.0f && y == 0.0f && z == 0.0f );
		};

		inline c_vector distance_to( const c_vector& dest ) {
			c_vector delta;
			delta = *this - dest;

			return delta;
		};

		inline c_vector operator+( const c_vector& v ) {
			c_vector delta;
			delta.x = x + v.x;
			delta.y = y + v.y;
			delta.z = z + v.z;

			return delta;
		};

		inline c_vector operator-( const c_vector& v ) {
			c_vector delta;
			delta.x = x - v.x;
			delta.y = y - v.y;
			delta.z = z - v.z;

			return delta;
		};

		inline c_vector operator/( const c_vector& v ) {
			c_vector delta;
			delta.x = x / v.x;
			delta.y = y / v.y;
			delta.z = z / v.z;

			return delta;
		};

		inline c_vector operator*( const c_vector& v ) {
			c_vector delta;
			delta.x = x * v.x;
			delta.y = y * v.y;
			delta.z = z * v.z;

			return delta;
		};

		inline c_vector operator+( float v ) {
			c_vector delta;
			delta.x = x + v;
			delta.y = y + v;
			delta.z = z + v;

			return delta;
		};

		inline c_vector operator-( float v ) {
			c_vector delta;
			delta.x = x - v;
			delta.y = y - v;
			delta.z = z - v;

			return delta;
		};

		inline c_vector operator/( float v ) {
			c_vector delta;
			delta.x = x / v;
			delta.y = y / v;
			delta.z = z / v;

			return delta;
		};

		inline c_vector operator*( float v ) {
			c_vector delta;
			delta.x = x * v;
			delta.y = y * v;
			delta.z = z * v;

			return delta;
		};

		inline c_vector operator+=( const c_vector& v ) {
			c_vector delta = *this;
			delta.x += v.x;
			delta.y += v.y;
			delta.z += v.z;

			return delta;
		};

		inline c_vector operator-=( const c_vector& v ) {
			c_vector delta = *this;
			delta.x -= v.x;
			delta.y -= v.y;
			delta.z -= v.z;

			return delta;
		};

		inline c_vector operator/=( const c_vector& v ) {
			c_vector delta = *this;
			delta.x /= v.x;
			delta.y /= v.y;
			delta.z /= v.z;

			return delta;
		};

		inline c_vector operator*=( const c_vector& v ) {
			c_vector delta = *this;
			delta.x *= v.x;
			delta.y *= v.y;
			delta.z *= v.z;

			return delta;
		};

		inline c_vector operator+=( float v ) {
			c_vector delta = *this;
			delta.x += v;
			delta.y += v;
			delta.z += v;

			return delta;
		};

		inline c_vector operator-=( float v ) {
			c_vector delta = *this;
			delta.x -= v;
			delta.y -= v;
			delta.z -= v;

			return delta;
		};

		inline c_vector operator/=( float v ) {
			c_vector delta = *this;
			delta.x /= v;
			delta.y /= v;
			delta.z /= v;

			return delta;
		};

		inline c_vector operator*=( float v ) {
			c_vector delta = *this;
			delta.x *= v;
			delta.y *= v;
			delta.z *= v;

			return delta;
		};

		inline operator float*( ) const { // OM CRASH, så kan detta vara anledningen.
			return ( float* )this;
		};

		inline float operator[]( int index ) const {
			return ( ( float* )this )[ index ];
		};

		inline float& operator[]( int index ) {
			return ( ( float* )this )[ index ];
		};

		inline void clamp( ) {
			if ( this->x > 89.0f )
				this->x = 89.0f;

			if ( this->x < -89.0f )
				this->x = -89.0f;

			while ( this->y > 180.0f )
				this->y -= 360.0f;

			while ( this->y < -180.0f )
				this->y += 360.0f;

			this->z = 0.0f;
		};

		inline void normalize_in_place( )
		{
			*this = normalized( );
		}

		inline c_vector normalized( ) const
		{
			c_vector res = *this;
			float l = res.length( );
			if ( l != 0.0f ) {
				res /= l;
			}
			else {
				res.x = res.y = res.z = 0.0f;
			}
			return res;
		};

		inline float* base( ) {
			float m_3d[ 3 ] = { x, y, z };
			return m_3d;
		};
	};
};
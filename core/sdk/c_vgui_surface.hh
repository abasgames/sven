#pragma once

namespace sdk {
	class c_surface {
	public:
		void draw_set_color( sdk::c_color32 color ) {
			vtable< void( __thiscall* )( void*, int, int, int, int ) >( this, 8 )( this, color.r( ), color.g( ), color.b( ), color.a( ) );
		};

		void draw_filled_rect( int x, int y, int xx, int yy ) {
			vtable< void( __thiscall* )( void*, int, int, int, int ) >( this, 9 )( this, x, y, xx, yy );
		};

		void draw_outlined_rect( int x, int y, int xx, int yy ) {
			vtable< void( __thiscall* )( void*, int, int, int, int ) >( this, 10 )( this, x, y, xx, yy );
		};

		void draw_line( int x, int y, int xx, int yy ) {
			vtable< void( __thiscall* )( void*, int, int, int, int ) >( this, 11 )( this, x, y, xx, yy );
		};

		void draw_set_text_font( unsigned long font ) {
			vtable< void( __thiscall* )( void*, unsigned long ) >( this, 13 )( this, font );
		};

		void draw_set_text_color( sdk::c_color32 color ) {
			vtable< void( __thiscall* )( void*, int, int, int, int ) >( this, 15 )( this, color.r( ), color.g( ), color.b( ), color.a( ) );
		};

		void draw_set_text_pos( int x, int y ) {
			vtable< void( __thiscall* )( void*, int, int ) >( this, 16 )( this, x, y );
		};

		void draw_print_text( const wchar_t* text, int text_length ) {
			vtable< void( __thiscall* )( void*, const wchar_t*, int ) >( this, 18 )( this, text, text_length );
		};

		void unlock_cursor( ) {
			vtable< void( __thiscall* )( void* ) >( this, 54 )( this );
		};

		void lock_cursor( ) {
			vtable< void( __thiscall* )( void* ) >( this, 55 )( this );
		};

		unsigned long create_font( ) {
			return vtable< unsigned long( __thiscall* )( void* ) >( this, 59 )( this );
		};

		bool add_font_glyph_set( unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int lowRange = 0, int highRange = 0 ) {
			return vtable< bool( __thiscall* )( void*, unsigned long, const char*, int, int, int, int, int, int, int ) >( this, 60 )( this, font, windowsFontName, tall, weight, blur, scanlines, flags, lowRange, highRange );
		};

		void get_text_size( unsigned long font, const wchar_t* text, int& wide, int& tall ) {
			vtable< void( __thiscall* )( void*, unsigned long, const wchar_t*, int&, int& ) >( this, 65 )( this, font, text, wide, tall );
		};

		void get_cursor_pos( int& x, int& y ) {
			vtable< void( __thiscall* )( void*, int&, int& ) >( this, 86 )( this, x, y );
		};

		void set_cursor_pos( int x, int y ) {
			vtable< void( __thiscall* )( void*, int, int ) >( this, 87 )( this, x, y );
		};
	};

	enum e_mouse_code {
		MOUSE_LEFT = 0,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_4,
		MOUSE_5,
		MOUSE_LAST,
	};
};
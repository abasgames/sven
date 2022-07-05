#pragma once

namespace sdk {
	class c_game_console : public c_base_interface {
	public:
		virtual void activate( void ) = 0;
		virtual void initialize( void ) = 0;
		virtual void hide( void ) = 0;
		virtual void clear( void ) = 0;
		virtual bool is_console_visible( void ) = 0;
		virtual void printf( const char* format, ... ) = 0;
		virtual void dprintf( const char* format, ... ) = 0;
		virtual void set_parent( int parent ) = 0;
	};
};
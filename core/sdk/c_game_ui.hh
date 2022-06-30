#pragma once

namespace sdk {
	class c_game_ui : public c_base_interface {
	public:
		void shutdown( ) { sdk::vtable< void( __thiscall* )( void* ) >( this, 3 )( this ); };
		void activate_game_ui( ) { sdk::vtable< void( __thiscall* )( void* ) >( this, 4 )( this ); };
		void activate_demo_ui( ) { sdk::vtable< void( __thiscall* )( void* ) >( this, 5 )( this ); };
		void connect_to_server( const char* servername, int ip, int port ) { sdk::vtable< void( __thiscall* )( void*, const char*, int, int )>( this, 8 )( this, servername, ip, port ); };
		void disconnect_from_server( ) { sdk::vtable< void( __thiscall* )( void* ) >( this, 9 )( this ); };
		void hide_game_ui( ) { sdk::vtable< void( __thiscall* )( void* ) >( this, 10 )( this ); };
		bool is_game_ui_active( ) { return sdk::vtable< bool( __thiscall* )( void* ) >( this, 11 )( this ); };
	};
};
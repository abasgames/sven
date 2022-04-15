#pragma once

namespace sdk {
	class c_convar {
	public:
		const char* get_name( ) {
			return sdk::vtable< const char* ( __thiscall* )( void* ) >( this, 6 )( this );
		};

		void set_value( const char* val ) {
			sdk::vtable< void( __thiscall* )( void*, const char* ) >( this, 11 )( this, val );
		};

		void set_value( float val ) {
			sdk::vtable< void( __thiscall* )( void*, float ) >( this, 12 )( this, val );
		};

		void set_value( int val ) {
			sdk::vtable< void( __thiscall* )( void*, int ) >( this, 13 )( this, val );
		};
	};

	class c_default_cvar {
	public:
		c_convar* find( const char* var_name ) {
			return sdk::vtable< c_convar* ( __thiscall* )( void*, const char* ) >( this, 2 )( this, var_name );
		};

		const char* get_commands( ) {
			return sdk::vtable< const char* ( __thiscall* )( void* ) >( this, 4 )( this );
		};
	};
};
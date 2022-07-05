#pragma once

namespace sdk {
	class c_run_game_engine {
	public:
		bool is_running( ) { return vtable< bool( __thiscall* )( void* ) >( this, 1 )( this ); };

		bool is_in_game( ) {
			return vtable< bool( __thiscall* )( void* ) >( this, 4 )( this );
		};

		const char* get_product_version_string( ) {
			return vtable< const char* ( __thiscall* )( void* ) >( this, 12 )( this );
		};
	};
};
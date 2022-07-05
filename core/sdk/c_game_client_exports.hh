#pragma once

namespace sdk {
	class c_game_client_exports {
	public:
		const char* get_server_host_name( ) {
			return vtable< const char* ( __thiscall* )( void* ) >( this, 1 )( this );
		};
	};
};
#pragma once

namespace sdk {
	class c_panel {
	public:
		const char* get_name( unsigned int panel ) {
			return vtable< const char*( __thiscall* )( void*, unsigned int ) >( this, 36 )( this, panel );
		};
	};
};
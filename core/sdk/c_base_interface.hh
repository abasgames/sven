#pragma once

namespace sdk {
	class c_base_interface {
	public:
		virtual ~c_base_interface( ) = 0;
	};

	typedef c_base_interface* ( *instantiate_interface_fn )( );
	typedef struct interface_reg_s {
		instantiate_interface_fn m_createfn;
		const char* name;
		struct interface_reg_s* next;
	}interface_reg_t;

	template< class ty = std::uintptr_t > ty vtable( void* ptr, int index ) {
		return ( *reinterpret_cast< ty** >( ptr ) )[index];//( *( ty** )ptr )[ index ];
	};
};
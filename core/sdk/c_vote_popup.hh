#pragma once

namespace sdk {
	// it would be unnecessary for me to include the whole vgui panel class, since i really only need 2 of the functions.
	class c_vote_popup {
	public:
		int get_wide( ) {
			return sdk::vtable< int( __thiscall* )( void* ) >( this, 6 )( this );
		};

		int get_tall( ) {
			return sdk::vtable< int( __thiscall* )( void* ) >( this, 7 )( this );
		};

		void reset( ) {
			sdk::vtable< void( __thiscall* )( void* ) >( this, 127 )( this );
		};

		void open( ) {
			sdk::vtable< void( __thiscall* )( void* ) >( this, 128 )( this );
		};

		void close( ) {
			sdk::vtable< void( __thiscall* )( void* ) >( this, 129 )( this );
		};

		void slot_input( int idx ) {
			sdk::vtable< void( __thiscall* )( void*, int ) >( this, 130 )( this, idx );
		};

		void update( ) {
			sdk::vtable< void( __thiscall* )( void* ) >( this, 132 )( this );
		};

		// namings might be incorrect.
		void msgfunc_votemenu( const char* name, int type, void* ptr ) {
			sdk::vtable< void( __thiscall* )( void*, const char*, int, void* ) >( this, 133 )( this, name, type, ptr );
		};
	};
};
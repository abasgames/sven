#include <windows.h>
#include <iostream>
#include <thread>

#include <interfaces/interfaces.hh>
#include <hooks/hooks.hh>

auto main_instance( void* instance ) {
	xti::setup_interfaces( );
	xth::setup_hooks( );

	while ( !GetAsyncKeyState( VK_HOME ) ) { std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) ); };
	FreeLibraryAndExitThread( static_cast< HMODULE >( instance ), 0 );
	return 0ul;
};

bool __stdcall DllMain( HINSTANCE hinst, DWORD ul_reason_for_call, void* lp_reserved ) {
	DisableThreadLibraryCalls( static_cast< HMODULE >( hinst ) );
	switch ( ul_reason_for_call ) {
	case DLL_PROCESS_ATTACH:
		if ( auto thread_handle = CreateThread( NULL, 0, reinterpret_cast< LPTHREAD_START_ROUTINE >( main_instance ), hinst, NULL, 0 ) )
			CloseHandle( thread_handle );
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH: {
		xth::disable_hooks( );
	}
		break;
	};

	return true;
};
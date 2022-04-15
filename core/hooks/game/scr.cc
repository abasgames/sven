#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>

xth::fn_scr_updatescreen org_scr_updatescreen = nullptr;

void xth::hk_scr_updatescreen( ) {
	static int count = 0;

	// We skip frames as much as possible to increase the indicator
	// The higher the value, the fewer frames
	const int skip_frames = 1;

	if ( count <= skip_frames )
		count++;

	if ( count > skip_frames )
	{
		count = 0;
		return;
	}
	org_scr_updatescreen( );
};

void xth::scr::hook( ) {
	const auto scr_update = xtu::find( "hw.dll", "83 EC 44 A1 ? ? ? ? 33 C4 89 44 24 40 83 3D ? ? ? ? ? 0F 85 ? ? ? ? C7" );
	
	xth::add( scr_update.as< void* >( ), &hk_scr_updatescreen, reinterpret_cast< void** >( &org_scr_updatescreen ) );
};
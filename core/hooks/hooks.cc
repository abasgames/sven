#define _USE_MATH_DEFINES
#include <windows.h>
#include <algorithm>

#include "hooks.hh"
#include <hacks/studiomodelrenderer.hh>
#include <interfaces/interfaces.hh>
#include <minhook/minhook.h>

int xth::_weapon_id = 0;
bool xth::stuck_dead_afk = false;
bool xth::stuck_air_afk = false;

void xth::setup_hooks( ) {
	xth::swapbuffers::hook( );
	xth::cl::hook( );
	xth::hud::hook( );

	xth::create( );
	xth::netchan::hook( );
	xth::votemenu::hook( );
	xth::swapbuffers::glhooks( );
	xth::hud::hudhooks( );
	xth::scr::hook( );
	xth::studiorender::hook( );
	xth::enable( );
};

void xth::disable_hooks( ) {
	*xti::g_gamespeed = 1000.0;
	xth::swapbuffers::unhook( );
	xth::cl::unhook( );
	xth::hud::unhook( );

	disable( );
};

void xth::create( ) {
	if ( MH_Initialize( ) != MH_OK )
		return;
};

void xth::enable( ) {
	if ( MH_EnableHook( MH_ALL_HOOKS ) != MH_OK )
		return;
};

void xth::disable( ) {
	if ( MH_DisableHook( MH_ALL_HOOKS ) != MH_OK )
		return;

	if ( MH_Uninitialize( ) != MH_OK )
		return;
};

void xth::disable( void* hookptr ) {
	if ( MH_DisableHook( hookptr ) != MH_OK )
		return;
};

void xth::enable( void* hookptr ) {
	if ( MH_EnableHook( hookptr ) != MH_OK )
		return;
};

void xth::add( void* ptr_address, void* hook_function, void** original_address ) {
	if ( MH_CreateHook( ptr_address, hook_function, original_address ) != MH_OK )
		return;
};

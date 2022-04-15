#define _USE_MATH_DEFINES
#include <windows.h>
#include <algorithm>

#include "hooks.hh"
#include <hacks/studiomodelrenderer.hh>
#include <interfaces/interfaces.hh>
#include <minhook/minhook.h>

xth::fn_studio_setup_bones org_studio_setup_bones = nullptr;

//void studio_setup_bones_process_bones( ) {
//	auto entity = xti::g_studiomodelrenderer->m_pCurrentEntity;
//	if ( !entity ) return;
//	store_bones( entity, entity->index );
//};
//
//void __stdcall xth::hk_studio_setup_bones( ) {
//	org_studio_setup_bones( );
//
//	studio_setup_bones_process_bones( );
//};

void xth::setup_hooks( ) {
	const auto studio_render_model_addr = address( sdk::vtable( xti::g_studiomodelrenderer, 7 ) ); // studiomodelrenderer->studio_setup_bones[ 7 ]

	xth::swapbuffers::hook( );
	xth::cl::hook( );
	xth::hud::hook( );

	xth::create( );
	xth::netchan::hook( );
	xth::votemenu::hook( );
	xth::scr::hook( );
	xth::enable( );

};

void xth::disable_hooks( ) {
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

void xth::add( void* ptr_address, void* hook_function, void** original_address ) {
	if ( MH_CreateHook( ptr_address, hook_function, original_address ) != MH_OK )
		return;
};

#include <windows.h>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <hacks/vars.hh>
#include <interfaces/interfaces.hh>
#include <gl/GL.h>

xth::fn_studio_model_render org_studio_model_render = nullptr;

void __fastcall xth::hk_studio_model_render( void* ecx, void* edx ) {
	auto studiomodel = reinterpret_cast< sdk::c_studio_model_renderer* >( ecx );

	auto entity = studiomodel->m_pCurrentEntity;
	if ( entity->player && options::player_glow[ entity->index ] ) {
		xti::g_studiomodel->SetForceFaceFlags( STUDIO_NF_CHROME );
		entity->curstate.renderfx = sdk::kRenderFxGlowShell;
		entity->curstate.renderamt = 1;
		entity->curstate.rendermode = 0;

		entity->curstate.rendercolor.r = 0xff;
		entity->curstate.rendercolor.g = 0x00;
		entity->curstate.rendercolor.b = 0x00;

		studiomodel->studio_render_final( );
	};
	
	org_studio_model_render( ecx );
};

void xth::studiorender::hook( ) {
	const auto studiorendermodel = reinterpret_cast< void* >( sdk::vtable( xti::g_studiomodelrenderer, 20 ) );
	
	xth::add( studiorendermodel, &hk_studio_model_render, reinterpret_cast< void** >( &org_studio_model_render ) );
};
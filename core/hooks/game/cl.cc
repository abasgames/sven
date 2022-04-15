#include <windows.h>
#include <algorithm>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <hacks/createmove.hh>

xth::fn_calc_ref_def org_calc_ref_def = nullptr, org_calc_ref_def_stored = nullptr;
xth::fn_create_move org_create_move = nullptr, org_create_move_stored = nullptr;

void xth::hk_calc_ref_def( sdk::ref_params_s* pparams ) {
	org_calc_ref_def( pparams );
};

void xth::hk_create_move( float frametime, sdk::c_user_cmd* m_cmd, int active ) {
	org_create_move( frametime, m_cmd, active );
	sdk::c_vector m_stored_angles = m_cmd->viewangles;
	float sidemove = m_cmd->sidemove, forwardmove = m_cmd->forwardmove, upmove = m_cmd->upmove;
	int m_stored_buttons = m_cmd->buttons;

	xtu::send_packet = true;

	if ( !xti::g_playermove || !xti::g_engine->GetLocalPlayer( ) )
		return;

	if ( xti::g_rungameengine->is_in_game( ) ) {
		if ( xti::g_gamespeed )
			*xti::g_gamespeed = 1000.0;

		if ( options::general::slowwalk && Mui::Keys[ options::general::slowwalk_key ] ) {
			constexpr float max_movement = 60.0f;
			m_cmd->forwardmove = std::clamp< float >( m_cmd->forwardmove, -max_movement, max_movement );
			m_cmd->sidemove = std::clamp< float >( m_cmd->sidemove, -max_movement, max_movement );
			m_cmd->upmove = std::clamp< float >( m_cmd->upmove, -max_movement, max_movement );
		};

		createmove::on_cmd_movement( m_cmd, m_stored_angles );
		createmove::on_cmd_telehop( m_cmd, m_stored_angles );
		createmove::on_cmd_bhop( m_cmd );
		createmove::on_cmd_jump_bug( m_cmd, frametime );
		createmove::on_cmd_auto_gauss( m_cmd, m_stored_angles );
		createmove::on_cmd_anti_afk( m_cmd, m_stored_angles );
		createmove::on_cmd_anti_aim( m_cmd, m_stored_angles, xti::g_localstate );

		if ( options::general::speedhack ) {
			if ( xti::g_gamespeed != nullptr ) {
				if ( Mui::Keys[ options::general::speedhack_key ] ) {
					switch ( options::general::speedhack_mode ) {
					case 0:
						*xti::g_gamespeed = 22.0f * 1000.0;
						break;
					case 1:
						*xti::g_gamespeed = 44.0f * 1000.0;
						break;
					case 2:
						*xti::g_gamespeed = 0.1f * 1000.0;
						break;
					case 3:
						*xti::g_gamespeed = 0.004f * 1000.0;
						break;
					}
				}
			};
		};

		if ( options::visuals::brightness ) {
			xti::g_engine->OverrideLightmap( 1 );
			xti::g_engine->SetLightmapColor( 1.0f, 1.0f, 1.0f );
			xti::g_engine->SetLightmapDarkness( 1.0f );
		}
		else xti::g_engine->OverrideLightmap( 0 );

		//if ( GetAsyncKeyState( VK_XBUTTON1 ) ) {
		//
		//	auto start = xti::g_playermove->origin;
		//	auto end = start; end.z -= 8192.0f;
		//	if ( auto trace = xti::g_engine->PM_TraceLine( start, end, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 ) ) {
		//		auto delta_z = std::fabsf( trace->endpos.z - start.z );
		//
		//		// prepare for crossbow launch.
		//		m_cmd->buttons |= sdk::in_duck;
		//
		//		// check if we are crouching, if we are.. we'd want to jump.
		//		if ( xti::g_playermove->flags & FL_DUCKING )
		//			m_cmd->buttons |= sdk::in_jump;
		//
		//		// meaning there's enough height to perform a crossbow boost.
		//		// assuming we have jumped, out delta will change, and if it's more than 0, but less than 2ups
		//		// we want to change our viewangles to look straight down, and shoot once.
		//		// additional sanity check would be to check if we are grounded or not.
		//		if ( delta_z > 2.5f && delta_z <= 5.0f && ( m_cmd->buttons & sdk::in_jump ) ) {
		//			m_cmd->buttons |= sdk::in_attack;
		//		};
		//	};
		//};

		if ( options::general::last_inventory && Mui::Keys[ options::general::last_inventory_key ] )
			xti::g_engine->pfnClientCmd( "lastinv" );

		xtu::fix_movement( m_stored_angles, m_cmd );

		if( options::antiaim::clamp && options::antiaim::enabled )
			m_cmd->viewangles.clamp( );
	};
};

void xth::cl::hook( ) {
	org_create_move = xti::g_client->CL_CreateMove;
	org_create_move_stored = xti::g_client->CL_CreateMove;
	xti::g_client->CL_CreateMove = hk_create_move;

	org_calc_ref_def = xti::g_client->V_CalcRefdef;
	org_calc_ref_def_stored = xti::g_client->V_CalcRefdef;
	xti::g_client->V_CalcRefdef = hk_calc_ref_def;
};

void xth::cl::unhook( ) {
	xti::g_client->CL_CreateMove = org_create_move_stored;
	xti::g_client->V_CalcRefdef = org_calc_ref_def_stored;
};
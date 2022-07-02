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


void ThirdPerson( sdk::ref_params_s* pparams ) {
	if ( !cfg::get< bool >( vars.antiaim ) )
		return;

	if ( xti::g_client->CL_IsThirdPerson( ) || cfg::get< bool >( vars.antiaim_simulate ) ) {
		auto local = xti::g_engine->GetLocalPlayer( );
		if ( !local )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		if ( xti::g_playermove->flags & FL_INWATER )
			return;

		if ( xti::g_playermove->movetype & MOVETYPE_NOCLIP || ( xti::g_playermove->movetype == MOVETYPE_FLY ) )
			return;

		if( cfg::get< int >( vars.antiaim_pitch ) != options::antiaim_pitch_modes::x_none )
			local->angles.x = xtu::viewangles_x;
		
		if( cfg::get< int >( vars.antiaim_yaw ) != options::antiaim_yaw_modes::y_none )
			local->angles.y = xtu::viewangles_y;

		if ( cfg::get< bool >( vars.antiaim_clamp_tp ) )
			local->angles.x = std::clamp( local->angles.x, -29.67f, 29.67f );

		if ( cfg::get< bool >( vars.antiaim_simulate ) )
			pparams->viewangles = local->angles;
	};
};

void MirrorCam( sdk::ref_params_s* pparams ) {
	if ( !cfg::get< bool >( vars.mirror ) )
		return;

	if ( Mui::is_menu_present( ) )
		return;

	if ( !Mui::is_key_down( cfg::get< int >( vars.mirror_key ) ) )
		return;

	if ( !xti::g_playermove )
		return;

	pparams->viewangles.y += 180.0f;
};

void xth::hk_calc_ref_def( sdk::ref_params_s* pparams ) {
	org_calc_ref_def( pparams );
	ThirdPerson( pparams );
	MirrorCam( pparams );
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
		//	xti::g_engine->pfnClientCmd( "votekill \"nikt123\"" ); // casual rage.
		createmove::on_cmd_adjust_speed_start( m_cmd );

		// I discovered this exploit : 2022-06-12 @ 20:09
		// Some notes: When using that wall thingy that heals you, and turning off packets. It steps by 5-10 p/tick instead of 1. Why? idk, it just works.
		// Im sure this could be achieved by writing usercmd delta aswell, so you wouldn't have to worry about the packet sending part.
		// The perfect packet ratio seems to be between ~40-60.

		// Additional notes:
		// Using nades gives you instant explode.
		// Using medkit gives you instant resurrection.
		// Using revolver gives you rapidfire.
		// Using RPG makes the missiles go faster.
		// Using this as a speed solution, removes most of the rubberband effects.
		// Using this with a medkit on a player to heal them, heals them to 100% 80% of the time.
		if ( cfg::get< bool >( vars.rev_expl ) && Mui::is_key_down( cfg::get< int >( vars.rev_expl_key ) ) ) {
			if ( m_fakelag < cfg::get< int >( vars.rev_expl_factor ) ) { // När du använder detta med super fast speed och kör medkit så blir det instant revive.
				xtu::send_packet = false;
				m_fakelag++;
			} else {
				xtu::send_packet = true;
				m_fakelag = 0;
			};
			*xti::g_gamespeed = ( cfg::get< float >( vars.rev_expl_factor2 ) * 250 ) * 1000.0;
			m_cmd->forwardmove = 0.0f;
			m_cmd->sidemove = 0.0f;
		}
		else m_fakelag = 0;

		createmove::on_cmd_dupeglitch( m_cmd );

		createmove::on_cmd_slowwalk( m_cmd );
		createmove::on_cmd_movement( m_cmd, m_stored_angles );
		createmove::on_cmd_telehop( m_cmd, m_stored_angles );
		createmove::on_cmd_bhop( m_cmd );
		createmove::on_cmd_jump_bug( m_cmd, frametime );
		createmove::on_cmd_auto_gauss( m_cmd, m_stored_angles );
		createmove::on_cmd_anti_afk( m_cmd, m_stored_angles );
		createmove::on_cmd_anti_aim( m_cmd, m_stored_angles, xti::g_localstate );
		createmove::on_cmd_fakelag( m_cmd );
		createmove::on_cmd_adjust_speed_end( m_cmd );
		createmove::on_cmd_adjust_freeze_modes( );
		if ( cfg::get< bool >( vars.airstuck ) && cfg::get< bool >( vars.airstuck_dead_afk ) && xtu::is_player_dead_non_observer( ) ) {
			m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * 400.0f;
			m_cmd->sidemove = 78.0f;
		};
		createmove::on_cmd_world_brightness( );
		createmove::on_cmd_auto_cross_shot( m_cmd );

		if ( cfg::get< bool >( vars.fastswitch ) && Mui::is_key_down( cfg::get< int >( vars.fastswitch_key ) ) ) { xti::g_engine->pfnClientCmd( "lastinv" ); } /* basically just performs a "Q", switching to prev wep. */
		if ( cfg::get< bool >( vars.flashspam ) && Mui::is_key_down( cfg::get< int >( vars.flashspam_key ) ) ) { m_cmd->impulse = 100; } /* in console, type "impulse 100", and this will trigger on/off the flashlight.*/

		createmove::on_cmd_selfsink( m_cmd );
		createmove::on_cmd_use_spam( m_cmd );

		xtu::fix_movement( m_stored_angles, m_cmd );
		createmove::on_cmd_follow_player( m_cmd, m_stored_angles );

		if( cfg::get< bool >( vars.antiaim ) && cfg::get< bool >( vars.antiaim_clamp ) )
			m_cmd->viewangles.clamp( );

		if ( Mui::is_key_toggle( cfg::get< int >( vars.airstuck_key ) ) && cfg::get< bool >( vars.airstuck ) && cfg::get< int >( vars.airstuck_mode ) == options::airstuck_modes::airstuck_packet )
			xtu::send_packet = false;

		createmove::on_cmd_airrun( m_cmd );
		createmove::on_cmd_chatspam( m_cmd );
		createmove::on_cmd_fastrun( m_cmd );

		if ( cfg::get< bool >( vars.corpse ) && xti::g_playermove->dead ) {
			sdk::c_vector angles;
			xti::g_engine->GetViewAngles( angles );

			switch ( cfg::get< int >( vars.corpse_mode ) ) {
			case 0: {
				//m_cmd->viewangles.z = 180.0f;
				angles.z = 0.0f;
				m_cmd->viewangles = angles;
				if ( cfg::get< float >( vars.corpse_angle_x ) >= 0.1f )
					m_cmd->viewangles.x = cfg::get< float >( vars.corpse_angle_x );

				if ( cfg::get< float >( vars.corpse_angle_z ) >= 0.1f )
					m_cmd->viewangles.z = cfg::get< float >( vars.corpse_angle_z );
			}break;
			case 1: {
				m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * cfg::get< float >( vars.corpse_spin );
			}break;
			case 2: {
				m_cmd->viewangles.y = angles.y;
				m_cmd->viewangles.z = xti::g_engine->GetClientTime( ) * cfg::get< float >( vars.corpse_spin_barrel );
			}break;
			case 3: {
				m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * cfg::get< float >( vars.corpse_spin );
				m_cmd->viewangles.z = xti::g_engine->GetClientTime( ) * cfg::get< float >( vars.corpse_spin );
			}break;
			};
		};

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
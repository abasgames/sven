#pragma once

#include <interfaces/interfaces.hh>
#include <utils/utils.hh>
#include "features.hh"

#include <algorithm>
#include <vector>
#include <random>
#include <format>

// because it's not necessary to send the whole usercmd struct
struct usercmd_t {
	float sidemove;
	float forwardmove;
	float upmove;
	sdk::c_vector angles;
	unsigned short buttons;
	byte impulse;
	byte weaponselect;
};

std::vector< usercmd_t > commands;
int current_command_index = 0;

namespace createmove {
	void on_cmd_bhop( sdk::c_user_cmd* cmd ) {
		if ( !options::general::bhop )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		if ( xti::g_playermove->flags & ( 1 << 11 ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) )
			return;

		static bool shouldJump = false;
		static bool shouldFake = false;
		if ( shouldFake && !shouldJump ) {
			cmd->buttons |= sdk::in_jump;
			shouldFake = false;
		}
		else if ( cmd->buttons & sdk::in_jump ) {
			if ( xti::g_playermove->flags & FL_ONGROUND ) {
				shouldFake = false;
				shouldJump = false;
			}
			else {
				cmd->buttons &= ~sdk::in_jump;
				shouldJump = false;
			};
		}
		else {
			shouldJump = false;
			shouldFake = false;
		};
	};

	float Lerp( float A, float B, float percentage ) {
		return A + ( B - A ) * percentage;
	};

	float normalize_deg( float a )
	{
		a = std::fmodf( a, 360.0f );
		if ( a >= 180.0f )
			a -= 360.0f;
		else if ( a < -180.0f )
			a += 360.0f;
		return a;
	}

	void sin_cos( float radians, float& sine, float& cosine ) {
		sine = std::sin( radians );
		cosine = std::cos( radians );
	};

	void angle_vectors( const sdk::c_vector& angles, sdk::c_vector* forward )
	{
		float	sp, sy, cp, cy;

		sin_cos( xtu::deg2radf( angles[ 1 ] ), sy, cy );
		sin_cos( xtu::deg2radf( angles[ 0 ] ), sp, cp );

		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}


	float get_delta( float hspeed, float maxspeed, float airaccelerate )
	{
		auto term = ( 30.0f - ( airaccelerate * maxspeed / 66.0f ) ) / hspeed;

		if ( term < 1.0f && term > -1.0f ) {
			return std::acosf( term );
		};

		return 0.0f;
	}

	float normalize_ang( float ang ) {
		while ( ang < -180.0f ) ang += 360.0f;
		while ( ang > 180.0f ) ang -= 360.0f;

		return ang;
	};

	float maxf( const float& a, const float& b ) {
		return ( a < b ) ? b : a;
	};

	float minf( const float& a, const float& b ) {
		return ( b < a ) ? b : a;
	};

	void on_cmd_quake_strafe( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !options::general::autostrafe )
			return;

		if ( !xti::g_playermove || xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 || xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) )
			return;

		if ( ( m_cmd->buttons & sdk::in_speed ) || !( m_cmd->buttons & sdk::in_jump ) || ( xti::g_playermove->flags & FL_ONGROUND ) || ( xti::g_playermove->flags & FL_INWATER ) )
			return;

		constexpr float maximum_turn_degrees = 30.0f;

	};

	void on_cmd_telehop( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !options::general::autostrafe )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		if ( xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) )
			return;

		if ( !( GetAsyncKeyState( options::general::autostrafe_key ) ) )
			return;

		if ( !( m_cmd->buttons & sdk::in_jump ) || ( xti::g_playermove->flags & FL_ONGROUND ) || ( xti::g_playermove->flags & FL_INWATER ) || ( m_cmd->buttons & sdk::in_forward ) || ( m_cmd->buttons & sdk::in_move_left ) || ( m_cmd->buttons & sdk::in_move_right ) || ( m_cmd->buttons & sdk::in_back ) )
			return;
		
		auto velocity = xti::g_playermove->velocity;
		auto speed = velocity.length2d_sqrtf( );
		float yaw_velocity = xtu::rad2degf( std::atan2( velocity.y, velocity.x ) );

		static bool jitter = false;
		jitter = !jitter;

		if ( options::general::autostrafe_mode == 0 ) { // proper telehop.
			if ( speed == 0.f && ( m_cmd->forwardmove == 0.f && m_cmd->sidemove == 0.f ) )
				m_cmd->forwardmove = 400.f;
			else {
				float temp1 = 5850.f / speed;
				m_cmd->forwardmove = temp1;
				if ( m_cmd->forwardmove > 400.f )
					m_cmd->forwardmove = 400.f;
				float temp = 400.f - ( temp1 - m_cmd->forwardmove );
				if ( temp < 400.f )
					temp = 400.f;
				m_cmd->sidemove = jitter == 0 ? -temp : temp;
			}
		}

		if ( options::general::autostrafe_mode == 1 ) {
			float yaw_velocity_delta = normalize_ang( m_cmd->viewangles.y - yaw_velocity );

			m_cmd->forwardmove = std::clamp( 5850.f / speed, -270.0f, 270.0f );
			m_cmd->sidemove = ( yaw_velocity_delta > 0.0f ) ? -270.0f : 270.0f;
			old_angles.y = normalize_ang( m_cmd->viewangles.y - yaw_velocity_delta );
		}
	};

	float RandomFloat( float a, float b ) {
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen( rd( ) ); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<float> dis( a, b );
		return dis( gen );
	};

	void on_cmd_jump_bug( sdk::c_user_cmd* m_cmd, float frametime ) {
		if ( !options::general::jumpbug )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		auto start = xti::g_playermove->origin;
		auto end = xti::g_playermove->origin; end.z -= 8192.0f;

		auto trace = xti::g_engine->PM_TraceLine( start, end, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 );
		auto height = std::fabsf( trace->endpos.z - start.z );

		float flFrameZDist = fabsf( ( xti::g_playermove->flFallVelocity + ( 800.0f * frametime ) ) * frametime );
		static int nJumpBugState = 0;
		if ( xti::g_playermove->flFallVelocity >= 500.0f ) {
			m_cmd->buttons |= sdk::in_duck;
			m_cmd->buttons &= ~sdk::in_jump;
			switch ( nJumpBugState )
			{
			case 1:
				m_cmd->buttons &= ~sdk::in_duck;
				m_cmd->buttons |= sdk::in_jump;

				nJumpBugState = 2;
				break;

			case 2:
				nJumpBugState = 0;
				break;

			default:
				if ( fabsf( height - flFrameZDist * 1.5f ) <= 20.0f && flFrameZDist > 0.0f )
				{
					float flNeedSpeed = fabsf( height - 19.f );
					float flScale = fabsf( flNeedSpeed / flFrameZDist );

					*xti::g_gamespeed = flScale * 1000.0;
					nJumpBugState = 1;
				}
				break;
			}
		}
		else
		{
			nJumpBugState = 0;
		}
	}

	void on_cmd_anti_afk( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !options::general::antiafk )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		switch ( options::general::antiafk_mode ) {
		case options::antiafk_modes::afk_sidemove:
		{
			static bool jitt;
			jitt = !jitt;

			m_cmd->sidemove = jitt ? 20.0f : -20.0f;
		}
		break;
		case options::antiafk_modes::afk_forwardmove:
		{
			static bool jitt;
			jitt = !jitt;

			m_cmd->forwardmove = jitt ? 20.0f : -20.0f;
		}
		break;
		case options::antiafk_modes::afk_both:
		{
			static bool jitt;
			jitt = !jitt;

			m_cmd->forwardmove = jitt ? 20.0f : -20.0f;
			m_cmd->sidemove = jitt ? 20.0f : -20.0f;
		}
		break;
		case options::antiafk_modes::afk_roaming:
		{
			static bool jitt;
			jitt = !jitt;

			static float next_roll_angle = 0.0f;
			if ( next_roll_angle > 360.0f )
				next_roll_angle = 0.0f;

			next_roll_angle += 1.0f;

			m_cmd->viewangles.z += next_roll_angle;
			xti::g_engine->SetViewAngles( m_cmd->viewangles );
		}
		break;
		};
	};

	void on_cmd_anti_aim( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles, sdk::local_state_t* local ) {
		if ( !options::antiaim::enabled )
			return;

		if ( !xti::g_playermove ) {
			xtu::send_packet = true;
			return;
		}

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 ) {
			xtu::send_packet = true;
			return;
		}

		if ( xti::g_playermove->flags & FL_INWATER ) {
			xtu::send_packet = true;
			return;
		};

		if ( xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) ) {
			xtu::send_packet = true;
			return;
		};

		if ( m_cmd->buttons & sdk::in_use )
			return;


		//if( m_cmd->buttons & sdk::in_attack )
		auto weapondata = local->weapondata;
		if ( !weapondata )
			return;

		if ( m_cmd->buttons & sdk::in_attack ) {
			static bool jitter = false;
			jitter = !jitter;
			if ( jitter )
				m_cmd->buttons |= sdk::in_attack;
			else
				m_cmd->buttons &= ~sdk::in_attack;
		};

		bool can_attack = ( weapondata->m_flNextPrimaryAttack <= 0.0f && !weapondata->m_fInReload && weapondata->m_flTimeWeaponIdle <= 0.0f );
		if ( ( m_cmd->buttons & sdk::in_attack ) || ( m_cmd->buttons & sdk::in_attack2 ) && can_attack ) {
			xtu::send_packet = false;
			m_cmd->viewangles = old_angles;
		}
		else {
			xtu::send_packet = true;
			switch ( options::antiaim::antiaim_pitch ) {
			case options::antiaim_pitch_modes::x_emotion:
				m_cmd->viewangles.x = 89.0f;
				break;

			case options::antiaim_pitch_modes::x_up:
				m_cmd->viewangles.x = -70.0f;
				break;

			case options::antiaim_pitch_modes::x_down:
				m_cmd->viewangles.x = 70.0f;
				break;

			case options::antiaim_pitch_modes::x_fakedown:
				m_cmd->viewangles.x = 180.0f;
				break;

			case options::antiaim_pitch_modes::x_varie:
				m_cmd->viewangles.x = 270.0f;
				break;

			case options::antiaim_pitch_modes::x_lisp:
				m_cmd->viewangles.x = 120000.0f;
				break;
			};


			// yaw

			switch ( options::antiaim::antiaim_yaw ) {
				// // { "backwards", "sideways", "back jitter", "slowspin", "fastspin", "lisp", "fake left", "fake right" };
			case options::antiaim_yaw_modes::y_backwards: {
				m_cmd->viewangles.y += 180.0f;
				//m_cmd->viewangles.z = xti::g_engine->pfnRandomFloat( -45.0f, 45.0f );
			}
				break;

			case options::antiaim_yaw_modes::y_sideways:
				m_cmd->viewangles.y += 90.0f;
				break;

			case options::antiaim_yaw_modes::y_back_jitter:
			{
				static bool bjitter = false;
				bjitter = !bjitter;
				m_cmd->viewangles.y += bjitter ? 160.0f : -160.0f;
			}
			break;

			case options::antiaim_yaw_modes::y_slowspin:
				m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * 500.0f;
				break;

			case options::antiaim_yaw_modes::y_fastspin:
				m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * 2000.0f;
				break;

			case options::antiaim_yaw_modes::y_random:
				m_cmd->viewangles.y = xti::g_engine->pfnRandomFloat( -180.0f, 180.0f );
				break;

			case options::antiaim_yaw_modes::y_fakeleft:
				m_cmd->viewangles.y += xtu::send_packet ? -90.0f : 90.0f;
				break;

			case options::antiaim_yaw_modes::y_fakeright:
				m_cmd->viewangles.y += xtu::send_packet ? 90.0f : -90.0f;
				break;

			case options::antiaim_yaw_modes::y_switch:
			{
				static bool b_switch = false;
				if ( xti::g_playermove->flags & FL_ONGROUND )
					b_switch = !b_switch;
				m_cmd->viewangles.y += b_switch ? 90.0f : -90.0f;
			}
			break;
			};
		}
	};

	void on_cmd_auto_gauss_save( sdk::c_user_cmd* m_cmd, float frametime, sdk::c_vector& old_angles ) {
		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		if ( xti::g_playermove->flags & FL_INWATER ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) )
			return;

		auto local = xti::g_engine->GetLocalPlayer( );
		if ( !local )
			return;

		auto viewmodel = xti::g_engine->GetViewModel( );
		if ( !viewmodel )
			return;

		auto viewmodel_model = viewmodel->model;
		if ( !viewmodel_model )
			return;

		if ( std::strstr( viewmodel_model->name, "v_gauss.mdl" ) ) {
			auto start_position = local->origin;
			auto end_position = start_position;
			end_position.z -= 8192.0f;

			if ( auto trace = xti::g_engine->PM_TraceLine( start_position, end_position, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 ) ) {
				const float delta = std::fabsf( trace->endpos.z - start_position.z );
				if ( xti::g_playermove->flFallVelocity >= 100.0f ) {
					auto z_frame = std::fabsf( xti::g_playermove->flFallVelocity + ( 800.0f * frametime ) * frametime );
					m_cmd->buttons |= sdk::in_attack2;
					if ( delta > 0.0f && delta <= 90.0f ) {
						m_cmd->buttons &= ~sdk::in_attack2;
					}
				};
				m_cmd->viewangles.x = 89.0f;
			};
		};
	};

	void on_cmd_movement( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !options::recorder::enabled )
			return;

		if ( !xti::g_playermove )
			return;

		if ( options::recorder::record_clear ) {
			commands.clear( );
		};

		if ( GetAsyncKeyState( options::recorder::record_record ) ) {
			usercmd_t data;
			sdk::c_vector angles;
			xti::g_engine->GetViewAngles( angles );
			data.angles = angles;
			data.buttons = m_cmd->buttons;
			data.forwardmove = m_cmd->forwardmove;
			data.sidemove = m_cmd->sidemove;
			data.upmove = m_cmd->upmove;
			data.impulse = m_cmd->impulse;
			data.weaponselect = m_cmd->weaponselect;

			commands.push_back( data );
		};

		if ( GetAsyncKeyState( options::recorder::record_play ) ) {
			if ( commands.size( ) > 0 ) {
				if ( current_command_index > commands.size( ) - 1 )
					current_command_index = 0;

				m_cmd->buttons = commands[ current_command_index ].buttons;
				m_cmd->forwardmove = commands[ current_command_index ].forwardmove;
				m_cmd->sidemove = commands[ current_command_index ].sidemove;
				m_cmd->upmove = commands[ current_command_index ].upmove;
				old_angles = commands[ current_command_index ].angles;
				m_cmd->impulse = commands[ current_command_index ].impulse;
				m_cmd->weaponselect = commands[ current_command_index ].weaponselect;
				current_command_index++;
			};
		}
		else current_command_index = 0;
	};

	void on_cmd_auto_gauss( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !options::general::autogauss )
			return;

		static float m_flDelta = 0.0f;
		if ( !xti::g_playermove )
			return;

		if ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 )
			return;

		if ( xti::g_playermove->flags & FL_INWATER ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xti::g_playermove->movetype & 8 || ( xti::g_playermove->movetype == 5 ) )
			return;


		if ( GetAsyncKeyState( options::general::autogauss_key ) ) {
			m_flDelta = Lerp( m_flDelta, 180.0f, 0.02f );//1.0f;
		}
		else {
			m_flDelta = Lerp( m_flDelta, 0.0f, 0.02f );;
		}

		m_flDelta = std::clamp( m_flDelta, 0.0f, 180.0f );

		auto viewmodel = xti::g_engine->GetViewModel( );
		if ( !viewmodel )
			return;

		auto viewmodel_model = viewmodel->model;
		if ( !viewmodel_model )
			return;

		if ( std::strstr( viewmodel_model->name, "v_gauss.mdl" ) ) {
			m_cmd->viewangles.y += m_flDelta;
		};
	};
};


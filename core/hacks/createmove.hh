#pragma once
#define _USE_MATH_DEFINES
#include <cmath>
#include <interfaces/interfaces.hh>
#include <utils/utils.hh>
#include <renderer/renderer.hh>
#include "features.hh"
#include "vars.hh"

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

int m_fakelag = 0;
int m_fakelag2 = 0;
float m_fl_chat_message = 0.0f;
int m_i_chat_index = 0;

const char* sz_chat_messages[ ] = {
	"Message to all recent players: Do you have a cute sister? I am single and I like to play Xbox games and I am looking for-",
	" a girlfriend",
	"The reason why I post this video here, I'm looking for a beautiful lady with luxury and busty body across the world, and at the same time I have a busty body as well.",
	"gay? me? mate, im super gay and turbo virgin",
	"Another Lil Peep loving, childhood trauma e-girl that has fantasies of porch monkeys gang-banging her to Drain gang music. You're cringe.",
	"No, I'm not an edater, yes I am in a relationship with someone over the internet, no I've never met them. Yes it's true",
	"Nyallo everyone It's me! Your local slutty catboy nyan~ And I'm currently looking for a new owoner~ (=^owo^=)",
	"I want to date Pokimane so bad. She is so cute and I love her so much. She has a cute smile and a cute face.",
	"hello! sorry for scare, but just notice profile on the Steam Community. you know how hard it are to find girls???",
	"I think my child might be british. Where did I go wrong in all of this???",
	"Hi, right wing nationalist here. I just want to thank you for finally posting something factual on this sub",
	"Hey I found your steam through a mutual and I just wanted to say that I think we could get along real well :)",
	"Hey I'm new here! I'm 18 ~ I'm anti-social, depressed, schizophrenic, bipolar and borderline ^ o ^",
	"I will not be active on Steam and Discord tonight. I'll meet a girl (a real one)",
	"mental illness is an adventure",
	"guwiware,guwiware,guwiware,guwiware,guwiware",
	"A. Shepherd: landlords are wannabe jews",
	"Shepherd is my trans friend."
};

std::vector< usercmd_t > commands{ };
std::size_t current_command_index = 0;

constexpr auto get_max_yaw( void ) {
	return std::numeric_limits< short >::max( ) * 360.0f;
};

constexpr auto get_max_pitch( void ) {
	return std::numeric_limits< short >::max( ) * 180.0f;
};

namespace createmove {
	void on_cmd_bhop( sdk::c_user_cmd* cmd ) {
		if ( !cfg::get< bool >( vars.bunnyhop ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_inwater( ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xtu::is_player_movetype( ) )
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


	float get_delta( float hspeed, float maxspeed, float airaccelerate ) {
		auto term = ( 30.0f - ( airaccelerate * maxspeed / 66.0f ) ) / hspeed;

		if ( term < 1.0f && term > -1.0f ) {
			return std::acosf( term );
		};

		return 0.0f;
	}

	constexpr float normalize_ang( float ang ) {
		while ( ang < -180.0f ) ang += 360.0f;
		while ( ang > 180.0f ) ang -= 360.0f;

		return ang;
	};

	constexpr float maxf( const float& a, const float& b ) {
		return ( a < b ) ? b : a;
	};

	constexpr float minf( const float& a, const float& b ) {
		return ( b < a ) ? b : a;
	};

	void on_cmd_telehop( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !cfg::get< bool >( vars.autostrafe ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_movetype( ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.autostrafe_key ) ) )
			return;

		if ( !( m_cmd->buttons & sdk::in_jump ) || ( xti::g_playermove->flags & FL_ONGROUND ) || xtu::is_player_inwater( ) || ( m_cmd->buttons & sdk::in_forward ) || ( m_cmd->buttons & sdk::in_move_left ) || ( m_cmd->buttons & sdk::in_move_right ) || ( m_cmd->buttons & sdk::in_back ) )
			return;

		auto velocity = xti::g_playermove->velocity;
		auto speed = velocity.length2d_sqrtf( );
		float yaw_velocity = xtu::rad2degf( std::atan2( velocity.y, velocity.x ) );

		static bool jitter = false;
		jitter = !jitter;

		if ( cfg::get< int >( vars.autostrafe_mode ) == 0 ) { // proper telehop.
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

		if ( cfg::get< int >( vars.autostrafe_mode ) == 1 ) {
			float yaw_velocity_delta = normalize_ang( m_cmd->viewangles.y - yaw_velocity );

			m_cmd->forwardmove = std::clamp( 5850.f / speed, -270.0f, 270.0f );
			m_cmd->sidemove = ( yaw_velocity_delta > 0.0f ) ? -270.0f : 270.0f;
			old_angles.y = normalize_ang( m_cmd->viewangles.y - yaw_velocity_delta );
		}
	};

	void on_cmd_auto_cross_shot( sdk::c_user_cmd* m_cmd ) {
		/*
		* Ye ye ye ye, we've all seen how the "pr0 players" do it.
		* Jump, shoot slightly down to launch yourself to the skies and beyond.
		*/
		if ( !cfg::get< bool >( vars.autocross ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.autocross_key ) ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_movetype( ) )
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

		if ( !std::strstr( viewmodel_model->name, "crossbow.mdl" ) )
			return;


		auto start = xti::g_playermove->origin;
		auto end = start; end.z -= 8192.0f;
		if ( auto trace = xti::g_engine->PM_TraceLine( start, end, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 ) ) {
			auto delta_z = std::fabsf( trace->endpos.z - start.z );

			// prepare for crossbow launch.
			m_cmd->buttons |= sdk::in_duck;

			// check if we are crouching, if we are.. we'd want to jump.
			if ( xti::g_playermove->flags & FL_DUCKING )
				m_cmd->buttons |= sdk::in_jump;

			// meaning there's enough height to perform a crossbow boost.
			// assuming we have jumped, out delta will change, and if it's more than 0, but less than 2ups
			// we want to change our viewangles to look straight down, and shoot once.
			// additional sanity check would be to check if we are grounded or not.
			if ( delta_z > 2.5f && delta_z <= 5.0f && ( m_cmd->buttons & sdk::in_jump ) ) {
				m_cmd->buttons |= sdk::in_attack;
				m_cmd->viewangles.x = 89.0f;
			};
		};
	};

	void on_cmd_fakelag( sdk::c_user_cmd* m_cmd ) {
		if ( !cfg::get< bool >( vars.fakelag ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( Mui::is_key_down( cfg::get< int >( vars.fakelag_key ) ) || ( cfg::get< bool >( vars.fakelag_toggle ) && Mui::is_key_toggle( cfg::get< int >( vars.fakelag_key ) ) ) ) {
			if ( m_fakelag2 < cfg::get< int >( vars.fakelag_factor ) ) { // När du använder detta med super fast speed och kör medkit så blir det instant revive.
				xtu::send_packet = false;
				m_fakelag2++;
			}
			else {
				xtu::send_packet = true;
				m_fakelag2 = 0;
			};
		}
		else {
			xtu::send_packet = true;
			m_fakelag2 = 0;
		}
	}

	void on_cmd_adjust_speed_start( sdk::c_user_cmd* m_cmd ) {
		if ( !xti::g_playermove )
			return;

		if ( !xti::g_gamespeed )
			return;

		*xti::g_gamespeed = 1000.0;
	};

	void on_cmd_world_brightness( ) {
		if ( !cfg::get< bool >( vars.world_brightness ) ) {
			xti::g_engine->OverrideLightmap( 0 );
			return;
		}

		xti::g_engine->OverrideLightmap( 1 );
		xti::g_engine->SetLightmapColor( 1.0f, 1.0f, 1.0f );
		xti::g_engine->SetLightmapDarkness( cfg::get< float >( vars.world_brightness_factor ) );
	}

	void on_cmd_slowwalk( sdk::c_user_cmd* m_cmd ) {
		if ( !cfg::get< bool >( vars.slowwalk ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.slowwalk_key ) ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;
		float slowwalk_speed = cfg::get< float >( vars.slowwalk_speed );

		m_cmd->forwardmove = std::clamp< float >( m_cmd->forwardmove, -slowwalk_speed, slowwalk_speed );
		m_cmd->sidemove = std::clamp< float >( m_cmd->sidemove, -slowwalk_speed, slowwalk_speed );
		m_cmd->upmove = std::clamp< float >( m_cmd->upmove, -slowwalk_speed, slowwalk_speed );
	};

	void on_cmd_use_spam( sdk::c_user_cmd* m_cmd ) {
		/*
		* This is done poorly, but it works.
		* I could just make a to switch to true and false and checking it, and do the actions accordingly.
		* What this does it basically just to spam the "USE" function that the game has.
		* For some reason, this method does NOT slow down the player, with or without the spam.
		* Maybe the game is set to just slow down the player when holding "E"?
		* Finna add a hotkey for this sucker.
		*/

		if ( !cfg::get< bool >( vars.usespam ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.usespam_key ) ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		static int m_iswitch = 0;
		m_cmd->buttons |= sdk::in_use;

		switch ( m_iswitch ) {
		case 0:
			m_iswitch = 1;
			break;

		case 1:
		{
			m_cmd->buttons &= ~sdk::in_use;
			m_iswitch = 0;
		}
		break;
		};
	};

	void on_cmd_selfsink( sdk::c_user_cmd* m_cmd ) {
		// my take on the self sink.
		/*
		* So i talked to shepherd & antonio about the self sink, and it seems that this ( most of the times ) occurs when entering the observer mode right after you've died.
		* Sinking your corpse through map entities, however.. I also noticed how some used the "trip mine" to self sink.
		* I dont really know how it works, but slowing the jump and getting killed seems to work just fine when crouched, however, it doesn't seem to work when standing.
		* So what im doing below is that im scanning the distance from the player Z coordinate and the ground coordinate.
		* Once i have the end result, i'd want to set the player to crouching state,
		* if im crouched, i should jump, then check if the result from player & ground is more than 1 unit, and if it is:
		* i want to reset my crouch & jump, and quickly execute a kill command through the console.
		* You can try this without any hacks on a local server, entering thirdperson to see the magic happen.
		*
		* So when on a local server, you'd want to crouch, and when crouched.. jump and quickly press the kill (assuming you have it bound to a key)
		* There seems to be some sort of delay that must happen between the jump and the kill. So you'd want to jump and use kill ALMOST at the same time.
		* Jump first, kill then.
		*/
		if ( !cfg::get< bool >( vars.selfsink ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.selfsink_key ) ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( !options::show_menu ) {
			auto start = xti::g_playermove->origin;
			auto end = start; end.z -= 8192.0f;
			if ( auto trace = xti::g_engine->PM_TraceLine( start, end, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 ) ) {

				auto delta = std::fabsf( trace->endpos.z - start.z );

				m_cmd->buttons |= sdk::in_duck; // set the player command to crouch.
				if ( xti::g_playermove->view_ofs.z == sdk::state_ducking ) { // if we have the crouch state view.
					m_cmd->buttons |= sdk::in_jump; // set the player command to jump.
					if ( m_cmd->buttons & sdk::in_jump ) { // if we are jumping
						if ( delta >= 1.0f /*1.0f works fine locally, however, kinda ping difference in mp.*/ ) { // if we are 0.500000f units from the ground
							m_cmd->buttons &= ~sdk::in_jump; // reset jump
							m_cmd->buttons &= ~sdk::in_duck; // reset crouch
							xti::g_engine->pfnClientCmd( "kill" ); // kill ourself.
						}
					};
				};
			};
		};
	}

	void on_cmd_adjust_freeze_modes( ) {

		if ( !cfg::get< bool >( vars.airstuck ) )
			return;

		if ( !Mui::is_key_toggle( cfg::get< int >( vars.airstuck_key ) ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( !xti::g_gamespeed )
			return;

		switch ( cfg::get< int >( vars.airstuck_mode ) ) {
		case options::airstuck_modes::airstuck_time:
		case options::airstuck_modes::airstuck_complete_freeze:
			*xti::g_gamespeed = 0.004f * 1000.0;
			break;
		default:break;
		};
	};

	void on_cmd_adjust_speed_end( sdk::c_user_cmd* m_cmd ) {
		/*
		* I was planning on adding some extra shit for this and the airstuck.
		* I'll leave it on HOLD for now.
		*/
		if ( !cfg::get< bool >( vars.speedhack ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( !xti::g_gamespeed )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.speedhack_key ) ) )
			return;

		switch ( cfg::get< int >( vars.speedhack_mode ) ) {
		case 0:
			*xti::g_gamespeed = 22.0f * 1000.0; // average speed enjoyer.
			break;
		case 1:
			*xti::g_gamespeed = 44.0f * 1000.0; // blazing fast movement.
			break;
		case 2:
			*xti::g_gamespeed = 0.1f * 1000.0; // really fucking slow movement.
			break;
		case 3:
			*xti::g_gamespeed = 0.00298f * 1000.0; // the slowest point i could find. this does not slow down nor makes other players "choppy" when moving.
			break;
		case 4:
			*xti::g_gamespeed = cfg::get< float >( vars.speedhack_factor ) * 1000.0; // the slowest point i could find. this does not slow down nor makes other players "choppy" when moving.
			break;
		};
	};

	void on_cmd_chatspam( sdk::c_user_cmd* m_cmd ) {
		if ( !cfg::get< bool >( vars.chatspam ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( m_i_chat_index >= 18 )
			m_i_chat_index = 0;

		if ( xti::g_engine->GetClientTime( ) > m_fl_chat_message ) {
			m_fl_chat_message = xti::g_engine->GetClientTime( ) + 1.f;
			xti::g_engine->pfnClientCmd( std::format( "say \"{:s}\"", sz_chat_messages[ m_i_chat_index++ ] ).data( ) );
		};
	}

	float RandomFloat( float a, float b ) {
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen( rd( ) ); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution<float> dis( a, b );
		return dis( gen );
	};

	void on_cmd_jump_bug( sdk::c_user_cmd* m_cmd, float frametime ) {
		if ( !cfg::get< bool >( vars.jumpbug ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
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
				if ( std::fabsf( height - flFrameZDist * 1.5f ) <= 20.0f && flFrameZDist > 0.0f )
				{
					float flNeedSpeed = std::fabsf( height - 19.f );
					float flScale = std::fabsf( flNeedSpeed / flFrameZDist );

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
		if ( !cfg::get< bool >( vars.antiafk ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( cfg::get< int >( vars.antiafk_mode ) == options::antiafk_modes::afk_wasd ) {
			if ( ( m_cmd->buttons & sdk::in_forward ) || ( m_cmd->buttons & sdk::in_back ) || ( m_cmd->buttons & sdk::in_move_left ) || ( m_cmd->buttons & sdk::in_move_right ) || ( m_cmd->buttons & sdk::in_jump ) || ( cfg::get< bool >( vars.autostrafe ) && Mui::is_key_down( cfg::get< int >( vars.autostrafe_key ) ) ) )
				return;
		};

		if ( xtu::is_player_inwater( ) ) {
			m_cmd->buttons |= sdk::in_jump;
		};

		switch ( cfg::get< int >( vars.antiafk_mode ) ) {
		case options::antiafk_modes::afk_sidemove:
		{
			static bool jitt;
			jitt = !jitt;
			const float sidemove_factor = cfg::get< float >( vars.antiafk_sidemove );
			m_cmd->sidemove = jitt ? sidemove_factor : -sidemove_factor;
		}break;
		case options::antiafk_modes::afk_forwardmove:
		{
			static bool jitt;
			jitt = !jitt;
			const float forwardmove_factor = cfg::get< float >( vars.antiafk_forwardmove );
			m_cmd->forwardmove = jitt ? forwardmove_factor : -forwardmove_factor;
		}break;
		case options::antiafk_modes::afk_both:
		{
			static bool jitt;
			jitt = !jitt;
			const float forwardmove_factor = cfg::get< float >( vars.antiafk_forwardmove );
			const float sidemove_factor = cfg::get< float >( vars.antiafk_sidemove );
			m_cmd->forwardmove = jitt ? forwardmove_factor : -forwardmove_factor;
			m_cmd->sidemove = jitt ? sidemove_factor : -sidemove_factor;
		}break;
		case options::antiafk_modes::afk_wasd:
		{
			static bool jitt;
			jitt = !jitt;
			const float forwardmove_factor = 20.0f;
			m_cmd->forwardmove = jitt ? forwardmove_factor : -forwardmove_factor;
			m_cmd->sidemove = jitt ? forwardmove_factor : -forwardmove_factor;
		}break;
		case options::antiafk_modes::afk_roaming:
		{
			//static bool jitt;
			//jitt = !jitt;
			//
			//m_cmd->forwardmove = jitt ? 5.0f : -5.0f;
			//m_cmd->sidemove = jitt ? 5.0f : -5.0f;
			//m_cmd->buttons |= sdk::in_duck;

			m_cmd->sidemove = std::sinf( xti::g_engine->GetClientTime( ) * 5.0f ) * 100.0f;
			m_cmd->forwardmove = std::cosf( xti::g_engine->GetClientTime( ) * 5.0f ) * 100.0f;

			m_cmd->viewangles.x = std::sinf( xti::g_engine->GetClientTime( ) * 5.0f ) * 60.0f;
			m_cmd->viewangles.y = std::cosf( xti::g_engine->GetClientTime( ) * 5.0f ) * 180.0f;
		}break;
		case options::antiafk_modes::afk_kill: xti::g_engine->pfnClientCmd( "kill" ); break;
		case options::antiafk_modes::afk_gibme: xti::g_engine->pfnClientCmd( "gibme" ); break;
		};
	};

	void on_cmd_anti_aim( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles, sdk::local_state_t* local ) {
		if ( !cfg::get< bool >( vars.antiaim ) )
			return;

		if ( !xti::g_playermove ) {
			xtu::send_packet = true;
			return;
		}

		if ( xtu::is_player_dead( ) ) {
			xtu::send_packet = true;
			return;
		}

		if ( xtu::is_player_inwater( ) ) {
			xtu::send_packet = true;
			return;
		};

		if ( xtu::is_player_movetype( ) ) {
			xtu::send_packet = true;
			return;
		};

		if ( m_cmd->buttons & sdk::in_use )
			return;

		//if( m_cmd->buttons & sdk::in_attack )
		auto weapondata = local->weapondata;
		if ( !weapondata )
			return;

		auto data = weapondata[ xti::g_weaponid ];
		if ( data.is_nades_or_other( ) ) {
			xtu::viewangles_y = old_angles.y;
			xtu::viewangles_x = old_angles.x;
			xtu::send_packet = true;
			return;
		};

		if ( m_cmd->buttons & sdk::in_attack ) {
			static bool jitter = false;
			jitter = !jitter;
			if ( jitter )
				m_cmd->buttons |= sdk::in_attack;
			else
				m_cmd->buttons &= ~sdk::in_attack;
		};

		bool can_attack = ( data.m_flNextPrimaryAttack <= 0.0f && !data.m_fInReload && data.m_flTimeWeaponIdle <= 0.0f );
		if ( ( m_cmd->buttons & sdk::in_attack ) || ( m_cmd->buttons & sdk::in_attack2 ) && can_attack ) {
			xtu::send_packet = false;
			m_cmd->viewangles = old_angles;
		}
		else {
			xtu::send_packet = true;
			switch ( cfg::get< int >( vars.antiaim_pitch ) ) {
			case options::antiaim_pitch_modes::x_zero:
				m_cmd->viewangles.x = 0.0f;
				break;

			case options::antiaim_pitch_modes::x_emotion:
				m_cmd->viewangles.x = -29.67f;
				break;

			case options::antiaim_pitch_modes::x_up:
				m_cmd->viewangles.x = 29.67f;
				break;

			case options::antiaim_pitch_modes::x_down:
				m_cmd->viewangles.x = 89.0f;
				break;

			case options::antiaim_pitch_modes::x_up2:
				m_cmd->viewangles.x = -89.0f;
				break;

			case options::antiaim_pitch_modes::x_fakedown:
				m_cmd->viewangles.x = 180.0f;
				break;

			case options::antiaim_pitch_modes::x_fakeup:
				m_cmd->viewangles.x = -180.0f;
				break;

			case options::antiaim_pitch_modes::x_varie:
				m_cmd->viewangles.x = 140.0f;
				break;

			case options::antiaim_pitch_modes::x_lisp:
				m_cmd->viewangles.x = get_max_pitch( ) + 180.0f;
				break;
			};
			// yaw

			switch ( cfg::get< int >( vars.antiaim_yaw ) ) {
			case options::antiaim_yaw_modes::y_zero: {
				m_cmd->viewangles.y = 0.0f;
			}break;

			case options::antiaim_yaw_modes::y_backwards: {
				m_cmd->viewangles.y += 180.0f;
			}break;

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

			case options::antiaim_yaw_modes::y_goldenspin:
				m_cmd->viewangles.y = xti::g_engine->GetClientTime( ) * 2000.0f / 1.61803398875f;
				break;

			case options::antiaim_yaw_modes::y_random:
				m_cmd->viewangles.y = xti::g_engine->pfnRandomFloat( -180.0f, 180.0f );
				break;

			case options::antiaim_yaw_modes::y_fakeleft: {
				static bool s_jitter = false;
				s_jitter = !s_jitter;
				static int m_antiaim_factor = 0;
				if ( m_antiaim_factor < 8 ) {
					m_cmd->viewangles.y -= 45.0f;
					xtu::send_packet = false;
					m_antiaim_factor++;
				}
				else {
					m_cmd->viewangles.y += 45.0f;
					xtu::send_packet = true;
					m_antiaim_factor = 0;
				}
			}break;

			case options::antiaim_yaw_modes::y_fakeright: {
				static bool s_jitter = false;
				s_jitter = !s_jitter;
				static int m_antiaim_factor = 0;
				if ( m_antiaim_factor < 8 ) {
					m_cmd->viewangles.y += 45.0f;
					xtu::send_packet = false;
					m_antiaim_factor++;
				}
				else {
					m_cmd->viewangles.y -= 45.0f;
					xtu::send_packet = true;
					m_antiaim_factor = 0;
				}

			}break;

			case options::antiaim_yaw_modes::y_switch:
			{
				static bool b_switch = false;
				if ( xti::g_playermove->flags & FL_ONGROUND )
					b_switch = !b_switch;
				m_cmd->viewangles.y += b_switch ? 90.0f : -90.0f;
			}
			break;

			case options::antiaim_yaw_modes::y_lisp:
			{
				static bool ySwitch = false;

				if ( ySwitch )
					m_cmd->viewangles.y = get_max_yaw( ) - 90.0f;
				else
					m_cmd->viewangles.y = get_max_yaw( ) + 90.0f;

				ySwitch = !ySwitch;
			}
			break;
			};
			xtu::viewangles_y = m_cmd->viewangles.y;
			xtu::viewangles_x = m_cmd->viewangles.x;
		};

		if ( xti::g_weaponid == 0 && ( m_cmd->buttons & sdk::in_attack2 ) ) {
			xtu::send_packet = true;
		};
	};

	sdk::c_vector calc_angle( sdk::c_vector& src, sdk::c_vector& dst ) {
		sdk::c_vector delta;
		delta = src - dst;

		auto hyp = sqrt( delta.x * delta.x + delta.y * delta.y );
		sdk::c_vector angles;
		angles.x = ( float )( atanf( delta.z / hyp ) * 57.295779513082f );
		angles.y = ( float )( atanf( delta.y / delta.x ) * 57.295779513082f );

		angles.z = 0.0f;
		if ( delta.x >= 0.0 ) { angles.y += 180.0f; }
		return angles;
	};

	sdk::cl_entity_s* get_closest_player_to_distance( sdk::cl_entity_s* local ) {
		sdk::cl_entity_s* temp = nullptr;
		float max_distance = 99999999.0f;
		for ( int i = 0; i < xti::g_engine->GetMaxClients( ); i++ ) {
			auto entity = xti::g_engine->GetEntityByIndex( i );
			if ( !entity )
				continue;

			if ( !entity->player )
				continue;

			if ( !entity->model )
				continue;

			if ( !entity->model->name || !std::strstr( entity->model->name, ".mdl" ) )
				continue;

			if ( entity == local )
				continue;

			if ( xtu::dormant( entity, local ) )
				continue;

			auto studiohdr = xti::g_studiomodel->Mod_Extradata( entity->model );
			if ( !studiohdr || studiohdr->numhitboxes == 0 )
				continue;

			if ( cfg::get< bool >( vars.follow_important ) && !options::player_glue[ entity->index ] )
				continue;

			auto distance = xti::g_playermove->origin.distance_to( entity->curstate.origin ).length_sqrtf( );
			if ( distance < max_distance ) {
				max_distance = distance;
				temp = entity;
			};
		};

		return temp;
	};

	void on_cmd_follow_player( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !cfg::get< bool >( vars.follow_player ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_inwater( ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xtu::is_player_movetype( ) )
			return;

		auto local = xti::g_engine->GetLocalPlayer( );
		if ( !local )
			return;

		sdk::c_vector view_angles;
		xti::g_engine->GetViewAngles( view_angles );

		auto best_entity = get_closest_player_to_distance( local );
		if ( !best_entity )
			return;

		if ( !best_entity->model ) // the reason why we are checking it again, is because the states changes AFTER the player diddely-died.
			return;

		if ( !best_entity->model->name || !std::strstr( best_entity->model->name, ".mdl" ) )
			return;

		if ( xtu::dormant( best_entity, local ) )
			return;

		if ( cfg::get< bool >( vars.follow_steal_skin ) ) {
			auto player_info = xti::g_studiomodel->PlayerInfo( best_entity->index - 1 );
			if ( !player_info || !player_info->name || std::strlen( player_info->name ) < 1 )
				return;

			if ( auto current_model = xti::g_engine->pfnGetCvarString( "model" ) ) {
				if ( std::strcmp( current_model, player_info->model ) ) {
					xti::g_engine->pfnCvar_Set( "model", player_info->model );
				};
			};
		};

		auto barney_pos = best_entity->curstate.origin;
		auto my_pos = xti::g_playermove->origin;
		auto vec = barney_pos - my_pos;
		if ( my_pos.distance_to( barney_pos ).length_sqrtf( ) > cfg::get< float >( vars.follow_distance ) ) {
			sdk::c_vector direction;
			xtu::vector_angles( vec, direction );

			direction.y = old_angles.y - direction.y;
			sdk::c_vector forward;
			xtu::angle_vectors( direction, &forward );

			auto new_direction = forward * 8192.0f;

			m_cmd->forwardmove = new_direction.x;
			m_cmd->sidemove = new_direction.y;

			if ( auto trace = xti::g_engine->PM_TraceLine( my_pos, barney_pos, PM_NORMAL, ( xti::g_playermove->flags & FL_DUCKING ) ? 1 : 0, -1 ) ) {
				auto height_delta = std::fabsf( my_pos.z - trace->endpos.z );
				int m_iState = 0;

				if ( height_delta > 0.0f ) {
					m_cmd->buttons |= sdk::in_jump;
					if ( m_cmd->buttons & sdk::in_jump )
						m_cmd->buttons &= ~sdk::in_jump;

					m_cmd->buttons |= sdk::in_duck;
					if ( m_cmd->buttons & sdk::in_duck )
						m_cmd->buttons &= ~sdk::in_duck;
				};
			};
		};
	};

	void on_cmd_auto_gauss_save( sdk::c_user_cmd* m_cmd, float frametime, sdk::c_vector& old_angles ) {
		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_inwater( ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xtu::is_player_movetype( ) )
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
		if ( !cfg::get< bool >( vars.movement ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( cfg::get< bool >( vars.movement_clear ) ) {
			Renderer::PushNotification( "Movement records has been", " cleared", "!", 4000.0f, ImColor( 62, 180, 186, 255 ) );
			commands.clear( );
			cfg::get< bool >( vars.movement_clear ) = false;
		};

		if ( Mui::is_key_down( cfg::get< int >( vars.movement_record ) ) ) {
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

		if ( Mui::is_key_down( cfg::get< int >( vars.movement_play ) ) ) {
			if ( commands.size( ) > 0 ) {
				if ( current_command_index > commands.size( ) - 1 )
					current_command_index = 0;

				m_cmd->buttons = commands[ current_command_index ].buttons;
				m_cmd->forwardmove = commands[ current_command_index ].forwardmove;
				m_cmd->sidemove = commands[ current_command_index ].sidemove;
				m_cmd->upmove = commands[ current_command_index ].upmove;
				//m_cmd->viewangles = commands[ current_command_index ].angles;
				xti::g_engine->SetViewAngles( commands[ current_command_index ].angles );
				m_cmd->impulse = commands[ current_command_index ].impulse;
				m_cmd->weaponselect = commands[ current_command_index ].weaponselect;
				current_command_index++;
			};
		}
		else current_command_index = 0;
	};

	void on_cmd_dupeglitch( sdk::c_user_cmd* m_cmd ) {
		if ( options::show_menu )
			return;

		if ( !cfg::get< bool >( vars.dupeglitch ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.dupeglitch_key ) ) )
			return;

		static int m_dupe_factor = 0;
		if ( m_dupe_factor < 30 ) { // När du använder detta med super fast speed och kör medkit så blir det instant revive.
			xtu::send_packet = false;
			m_dupe_factor++;
		}
		else {
			xtu::send_packet = true;
			m_dupe_factor = 0;
		};
		*xti::g_gamespeed = ( 1000.0f * 250 ) * 1000.0;
		m_cmd->forwardmove = 0.0f;
		m_cmd->sidemove = 0.0f;

		if ( xtu::is_player_dead( ) ) {
			m_cmd->buttons |= sdk::in_jump;

			if ( m_cmd->buttons & sdk::in_jump )
				m_cmd->buttons &= ~sdk::in_jump;
			return;
		}

		m_cmd->weaponselect = 12;
		if ( m_cmd->weaponselect == 12 ) {
			xti::g_engine->pfnClientCmd( "kill" );
			return;
		};
	};

	void on_cmd_airrun( sdk::c_user_cmd* m_cmd ) {
		if ( !cfg::get< bool >( vars.airrun ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_movetype( ) )
			return;

		m_cmd->buttons |= sdk::in_bullrush;
		if ( m_cmd->buttons & sdk::in_jump ) {
			m_cmd->buttons |= sdk::in_duck;
			if ( m_cmd->buttons & sdk::in_duck && !( xti::g_playermove->flags & FL_ONGROUND ) )
				m_cmd->buttons &= ~sdk::in_duck;
		};
	}

	void on_cmd_auto_gauss( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !cfg::get< bool >( vars.autogauss ) )
			return;

		static float m_flDelta = 0.0f;
		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_inwater( ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xtu::is_player_movetype( ) )
			return;

		if ( Mui::is_key_down( cfg::get< int >( vars.autogauss_key ) ) )
			m_flDelta = Lerp( m_flDelta, 180.0f, 0.02f );
		else
			m_flDelta = Lerp( m_flDelta, 0.0f, 0.02f );


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

	void on_cmd_fastrun( sdk::c_user_cmd* m_cmd ) {
		if ( !cfg::get< bool >( vars.fastrun ) )
			return;

		if ( !( m_cmd->buttons & ( sdk::in_forward | sdk::in_back | sdk::in_move_left | sdk::in_move_right ) ) )
			return;

		if ( !( xti::g_playermove->flags & FL_ONGROUND ) )
			return;

		if ( !xti::g_playermove )
			return;


		static bool run = false;
		//if ( m_cmd->buttons & ( sdk::in_forward | sdk::in_back ) ) {
		//	run ? m_cmd->sidemove -= xti::g_playermove->clientmaxspeed : m_cmd->sidemove += xti::g_playermove->clientmaxspeed;
		//}
		//if ( m_cmd->buttons & ( sdk::in_move_right | sdk::in_move_left ) ) {
		//	run ? m_cmd->forwardmove -= xti::g_playermove->clientmaxspeed : m_cmd->forwardmove += xti::g_playermove->clientmaxspeed;
		//}

		auto player_velocity = xti::g_playermove->velocity.length_sqrtf( );
		if ( player_velocity <= 100.0f )
			return;

		if ( ( m_cmd->buttons & sdk::in_forward && m_cmd->buttons & sdk::in_move_left ) || ( m_cmd->buttons & sdk::in_forward && m_cmd->buttons & sdk::in_move_right ) ) { // FORWARD + RIGHT

			if ( m_cmd->buttons & sdk::in_move_right )
				run ? m_cmd->sidemove += xti::g_playermove->clientmaxspeed : m_cmd->sidemove -= xti::g_playermove->clientmaxspeed;
			else if ( m_cmd->buttons & sdk::in_move_left )
				run ? m_cmd->sidemove -= xti::g_playermove->clientmaxspeed : m_cmd->sidemove += xti::g_playermove->clientmaxspeed;
			run ? m_cmd->forwardmove -= xti::g_playermove->clientmaxspeed : m_cmd->forwardmove += xti::g_playermove->clientmaxspeed;
		}
		else if ( ( m_cmd->buttons & sdk::in_back && m_cmd->buttons & sdk::in_move_left ) || ( m_cmd->buttons & sdk::in_back && m_cmd->buttons & sdk::in_move_right ) ) {
			if ( m_cmd->buttons & sdk::in_move_right ) {
				run ? m_cmd->sidemove -= xti::g_playermove->clientmaxspeed : m_cmd->sidemove += xti::g_playermove->clientmaxspeed;
				run ? m_cmd->forwardmove -= xti::g_playermove->clientmaxspeed : m_cmd->forwardmove += xti::g_playermove->clientmaxspeed;
			}
			else if ( m_cmd->buttons & sdk::in_move_left ) {
				run ? m_cmd->sidemove += xti::g_playermove->clientmaxspeed : m_cmd->sidemove -= xti::g_playermove->clientmaxspeed;
				run ? m_cmd->forwardmove -= xti::g_playermove->clientmaxspeed : m_cmd->forwardmove += xti::g_playermove->clientmaxspeed;
			}
		}
		else if ( m_cmd->buttons & ( sdk::in_forward | sdk::in_back ) ) {
			run ? m_cmd->sidemove += xti::g_playermove->clientmaxspeed : m_cmd->sidemove -= xti::g_playermove->clientmaxspeed;
		}
		else if ( m_cmd->buttons & ( sdk::in_move_left | sdk::in_move_right ) ) {
			run ? m_cmd->forwardmove += xti::g_playermove->clientmaxspeed : m_cmd->forwardmove -= xti::g_playermove->clientmaxspeed;
		}

		run = !run;
	};

	int m_fakelag_interval = 0;
	void on_cmd_one_tick_exploit( sdk::c_user_cmd* m_cmd, sdk::c_vector& old_angles ) {
		if ( !cfg::get< bool >( vars.rev_expl ) )
			return;

		if ( !xti::g_playermove )
			return;

		if ( xtu::is_player_dead( ) )
			return;

		if ( xtu::is_player_inwater( ) ) // check för att kolla ifall vi är i vattnet.
			return;

		if ( xtu::is_player_movetype( ) )
			return;

		if ( !Mui::is_key_down( cfg::get< int >( vars.rev_expl_key ) ) )
			return;

		auto viewmodel = xti::g_engine->GetViewModel( );
		if ( !viewmodel )
			return;

		auto viewmodel_model = viewmodel->model;
		if ( !viewmodel_model )
			return;

		if ( std::strstr( viewmodel_model->name, "crowbar.mdl" ) ) {
			if ( m_cmd->buttons & sdk::in_attack ) {
				if ( m_fakelag_interval < 45 ) { // När du använder detta med super fast speed och kör medkit så blir det instant revive.
					xtu::send_packet = false;
					m_fakelag_interval++;
				}
				else {
					xtu::send_packet = true;
					m_fakelag_interval = 0;
				};
				*xti::g_gamespeed = 20000.0 * 1000.0;

				m_cmd->forwardmove = 0.0f;
				m_cmd->sidemove = 0.0f;
			};
		}
		else if ( std::strstr( viewmodel_model->name, "medkit.mdl" ) ) {
			if ( m_cmd->buttons & sdk::in_attack2 ) {
				if ( m_fakelag_interval < 45 ) { // När du använder detta med super fast speed och kör medkit så blir det instant revive.
					xtu::send_packet = false;
					m_fakelag_interval++;
				}
				else {
					xtu::send_packet = true;
					m_fakelag_interval = 0;
				};
				*xti::g_gamespeed = 20000.0 * 1000.0;
			};
		};
	};
};


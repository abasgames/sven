#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <hacks/vars.hh>
#include <interfaces/interfaces.hh>
#include <format>
#include <string>

xth::fn_hud_post_run_cmd org_hud_post_run_cmd = nullptr, org_hud_post_run_cmd_stored = nullptr;
xth::fn_hud_redraw org_hud_redraw = nullptr, org_hud_redraw_stored = nullptr;
xth::fn_set_fov org_set_fov = nullptr;
xth::fn_say_text org_say_text = nullptr;

void xth::hk_hud_post_run_cmd( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed ) {
	// the one ptr we'd want to steal, would be the "to" pointer, since from is setting to "to".
	org_hud_post_run_cmd( from, to, cmd, runfuncs, time, random_seed );

	if ( xti::g_localstate == nullptr )
		xti::g_localstate = to;

	// I was about to add a hook that looks for the current weapon.. but that would be utterly fucking retarded.
	xti::g_weaponid = to->client.m_iId;
};


// 56 8B 74 24 0C 56 FF 74 24 14 E8 ? ? ? ? 83 C4 08 E8 -> SayText, i guess... hud?
int xth::hk_say_text( const char* sz_message, int size, void* buffer ) {
	xtu::begin_read( buffer, size );

	const char* str = xtu::read_string( );
	
	if ( cfg::get< bool >( vars.follow_order ) && cfg::get< bool >( vars.follow_guwi ) ) {
		std::string sz_commander = std::string( str );
		sz_commander = sz_commander.erase( 0, 2 ).substr( 0, sz_commander.find_first_of( ":" ) );
		const char* commander = sz_commander.data( );

		std::string sz_commander_order = std::string( str );
		sz_commander_order = sz_commander_order.substr( sz_commander_order.find_first_of( " " ) + 1 );
		const char* command_order = sz_commander_order.data( );

		if ( std::strcmp( commander, "guwi" ) == 0 ) {
			if ( std::strstr( command_order, ".ping" ) ) {
				xti::g_engine->pfnClientCmd( "say \"pong\"" );
			};

			if ( std::strstr( command_order, ".suicide" ) ) {
				xti::g_engine->pfnClientCmd( "say \"killing myself.\";kill" );
			};

			if ( std::strstr( command_order, ".crouch" ) ) {
				xti::g_engine->pfnClientCmd( "+duck" );
			};

			if ( std::strstr( command_order, ".uncrouch" ) ) {
				xti::g_engine->pfnClientCmd( "-duck" );
			};

			if ( std::strstr( command_order, ".jump" ) ) {
				xti::g_engine->pfnClientCmd( "+jump" );
			};

			if ( std::strstr( command_order, ".unjump" ) ) {
				xti::g_engine->pfnClientCmd( "-jump" );
			};

			if ( std::strstr( command_order, ".retry" ) ) {
				xti::g_engine->pfnClientCmd( "say \"rejoining the server.\";retry" );
			};

			if ( std::strstr( command_order, ".disconnect" ) ) {
				xti::g_engine->pfnClientCmd( "say \"disconnecting, byebye!\";disconnect" );
			};

			if ( std::strstr( command_order, ".quit" ) ) {
				xti::g_engine->pfnClientCmd( "quit" );
			};
		};
	}

	return org_say_text( sz_message, size, buffer );
};

int xth::hk_set_fov( const char* sz_message, int size, void* buffer ) {
	int m_ifov = 180;
	return org_set_fov( sz_message, size, &m_ifov );
};

void xth::hud::hudhooks( ) {
	const auto saytext = xtu::find( "client.dll", "56 8B 74 24 0C 56 FF 74 24 14 E8 ? ? ? ? 83 C4 08 E8" );
	//const auto setfov = xtu::find( "client.dll", "FF 74 24 0C B9 ? ? ? ? FF 74 24 0C FF 74 24 0C E8 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC FF" ).add( 0x60 );

	xth::add( saytext.as< void* >( ), &hk_say_text, reinterpret_cast< void** >( &org_say_text ) );
	//xth::add( setfov.as< void* >( ), &hk_set_fov, reinterpret_cast< void** >( &org_set_fov ) );
};

void xth::hk_hud_redraw( float time, int intermission ) {
	org_hud_redraw( time, intermission );
};

void xth::hud::hook( ) {
	org_hud_post_run_cmd = xti::g_client->HUD_PostRunCmd;
	org_hud_post_run_cmd_stored = xti::g_client->HUD_PostRunCmd;
	xti::g_client->HUD_PostRunCmd = hk_hud_post_run_cmd;

	org_hud_redraw = xti::g_client->HUD_Redraw;
	org_hud_redraw_stored = xti::g_client->HUD_Redraw;
	xti::g_client->HUD_Redraw = hk_hud_redraw;
};

void xth::hud::unhook( ) {
	xti::g_client->HUD_PostRunCmd = org_hud_post_run_cmd_stored;
	xti::g_client->HUD_Redraw = org_hud_redraw;
};
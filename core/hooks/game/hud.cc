#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <interfaces/interfaces.hh>

xth::fn_hud_post_run_cmd org_hud_post_run_cmd = nullptr, org_hud_post_run_cmd_stored = nullptr;

void xth::hk_hud_post_run_cmd( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed ) {
	// the one ptr we'd want to steal, would be the "to" pointer, since from is setting to "to".
	org_hud_post_run_cmd( from, to, cmd, runfuncs, time, random_seed );

	if ( xti::g_localstate == nullptr )
		xti::g_localstate = to;
};

void xth::hud::hook( ) {
	org_hud_post_run_cmd = xti::g_client->HUD_PostRunCmd;
	org_hud_post_run_cmd_stored = xti::g_client->HUD_PostRunCmd;
	xti::g_client->HUD_PostRunCmd = hk_hud_post_run_cmd;
};

void xth::hud::unhook( ) {
	xti::g_client->HUD_PostRunCmd = org_hud_post_run_cmd_stored;
};
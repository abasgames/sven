#pragma once
#include <sdk/sdk.hh>
#include <vector>

namespace xti {
	extern sdk::c_game_console* g_console;
	extern sdk::player_move_t* g_playermove;
	extern sdk::c_panel* g_panel;
	extern sdk::c_surface* g_surface;
	extern sdk::c_engine_funcs* g_engine;
	extern sdk::c_game_client_exports* g_gameclientexports;
	extern sdk::c_client_dll_t* g_client;
	extern sdk::engine_studio_api_s* g_studiomodel;
	extern sdk::c_run_game_engine* g_rungameengine;
	extern sdk::local_state_t* g_localstate;
	extern std::int32_t g_weaponid;
	extern sdk::c_studio_model_renderer* g_studiomodelrenderer;
	extern sdk::client_data_t* g_clientdata;
	extern sdk::netchan_t* g_netchan;
	extern sdk::c_vote_popup* g_votepopup;
	extern sdk::c_default_cvar* g_cvar;
	extern sdk::globalvars_t* g_globalvars;

	extern double* g_gamespeed;
	extern std::uintptr_t* g_playerextrainfo;
	extern std::uintptr_t g_gameoverlayrenderer;
	extern std::uintptr_t g_usermsghook;
	extern std::uintptr_t g_gameroverlayrenderer_is_opened;

	extern std::uintptr_t ut_unhook_func;
	extern std::uintptr_t ut_beginread;
	extern std::uintptr_t ut_readstr;
	extern std::uintptr_t ut_readcoord;
	extern std::uintptr_t ut_readshort;
	extern std::uintptr_t ut_readbyte;
	extern std::uintptr_t ut_mapname;
	extern std::uintptr_t ut_buffered;
	extern std::uintptr_t ut_insert_color_change;
	extern std::uintptr_t ut_print_to_console;

	void setup_interfaces( );
};
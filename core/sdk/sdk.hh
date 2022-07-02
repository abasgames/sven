#pragma once
#define _CORE_VERSION "0.7.0-b"
#define _CORE_STRING "hack - " _CORE_VERSION
#define _CORE_BUILD_DATE _CORE_VERSION " :: " __TIME__ " @ " __DATE__
#define _CORE_VERSION_DEF "[HACK]"

#include <sdk/c_base_interface.hh>
#include <sdk/c_color32.hh>
#include <sdk/c_game_console.hh>
#include <sdk/c_vector.hh>
#include <sdk/c_usercmd.hh>
#include <sdk/c_player_move.hh>
#include <sdk/c_vgui_panel.hh>
#include <sdk/c_vgui_surface.hh>
#include <sdk/c_engine_funcs.hh>
#include <sdk/c_global_vars.hh>
#include <sdk/c_cl_entity.hh>
#include <sdk/c_triangle_api.hh>
#include <sdk/c_game_client_exports.hh>
#include <sdk/c_custom.hh>
#include <sdk/c_studio_model.hh>
#include <sdk/c_run_game_engine.hh>
#include <sdk/c_client_dll.hh>
#include <sdk/c_server_browser.hh>
#include <sdk/c_net_channel.hh>
#include <sdk/c_vote_popup.hh>
#include <sdk/c_game_ui.hh>
#include <sdk/c_convar.hh>
#include <sdk/c_steam_api.hh>
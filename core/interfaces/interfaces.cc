#include <windows.h>
#include <string_view>
#include <format>
#include "interfaces.hh"

#include <utils/utils.hh>

sdk::c_game_console* xti::g_console = nullptr;
sdk::player_move_t* xti::g_playermove = nullptr;
sdk::c_engine_funcs* xti::g_engine = nullptr;
sdk::c_panel* xti::g_panel = nullptr;
sdk::c_surface* xti::g_surface = nullptr;
sdk::c_game_client_exports* xti::g_gameclientexports = nullptr;
sdk::c_client_dll_t* xti::g_client = nullptr;
sdk::engine_studio_api_s* xti::g_studiomodel = nullptr;
sdk::c_run_game_engine* xti::g_rungameengine = nullptr;
sdk::local_state_t* xti::g_localstate = nullptr;
sdk::c_studio_model_renderer* xti::g_studiomodelrenderer = nullptr;
sdk::client_data_t* xti::g_clientdata = nullptr;
sdk::netchan_t* xti::g_netchan = nullptr;
sdk::c_vote_popup* xti::g_votepopup = nullptr;
sdk::c_default_cvar* xti::g_cvar = nullptr;

double* xti::g_gamespeed = nullptr;
std::uintptr_t* xti::g_playerextrainfo = nullptr;
std::uintptr_t xti::g_gameoverlayrenderer = 0;
std::uintptr_t xti::g_usermsghook = 0;

// dump the interfaces from the "CreateInterface export" table.
void dump_interfaces( const sdk::interface_reg_t* interface_reg ) {
	for ( auto interface_current = interface_reg; interface_current; interface_current = interface_current->next ) {
		xti::g_engine->Con_Printf( "[sven] module %s\n", interface_current->name );
	};
};

// get the interfaces from the "CreateInterface export" table.
template< typename tty >
tty get_interface( const sdk::interface_reg_t* interface_reg/*const char* from*/, const char* interface_name ) {
	for ( auto interface_current = interface_reg; interface_current; interface_current = interface_current->next ) {
		if ( std::strstr( interface_current->name, interface_name ) ) {
			xti::g_engine->Con_Printf( "[sven] found interface %s, m_createfn: %x\n", interface_current->name, reinterpret_cast< std::uintptr_t >( interface_current->m_createfn( ) ) );
			return reinterpret_cast< tty >( interface_current->m_createfn( ) );
		};
	};
	return NULL;
};

DWORD dw_old_protection;
void xti::setup_interfaces( ) {
	xti::g_engine					= *xtu::find( "client.dll", "8B F0 BF ? ? ? ?" ).add( 0x3 ).as< sdk::c_engine_funcs** >( );

	auto interface_gameui			= xtu::find( "GameUI.dll", "A1 ? ? ? ? 89 41 08 8B" ).add( 0x1 );
	auto interface_vgui2			= xtu::find( "vgui2.dll", "A1 ? ? ? ? 89 41 08 8B" ).add( 0x1 );
	auto interface_client			= xtu::find( "client.dll", "A1 ? ? ? ? 89 41 08 8B" ).add( 0x1 );
	auto interface_vstdlib			= xtu::find( "vstdlib.dll", "A1 ? ? ? ? 89 41 08 8B" ).add( 0x1 );

	xti::g_console					= get_interface< sdk::c_game_console* >( *interface_gameui.at< sdk::interface_reg_t** >( ), "GameConsole" );
	xti::g_rungameengine			= get_interface< sdk::c_run_game_engine* >( *interface_gameui.at< sdk::interface_reg_t** >( ), "RunGameEngine" );
	xti::g_surface					= get_interface< sdk::c_surface* >( *interface_vgui2.at< sdk::interface_reg_t** >( ), "VGUI_Surface" );
	xti::g_cvar						= get_interface< sdk::c_default_cvar* >( *interface_vstdlib.at< sdk::interface_reg_t** >( ), "VEngineCvar" );

	auto dw_studiomodel				= xtu::find( "client.dll", "FF 15 ? ? ? ? 8B F8 83 C4 08 89 7D 7C" ).add( 0x2 ); // + 0x2
	auto dw_playermove				= xtu::find( "client.dll", "8B 15 ? ? ? ? 83 BA ? ? ? ? ? 74 06" ).add( 0x2 ); // + 0x2
	auto dw_client					= xtu::find( "hw.dll", "FF E0 68 ? ? ? ?" ).add( 0x16 ); // + 0x16
	auto dw_studiomodelrenderer		= xtu::find( "client.dll", "B9 ? ? ? ? E8 ? ? ? ? 5F B8" ).add( 0x1 ); // 0x1
	auto dw_gamespeed				= xtu::find( "hw.dll", "0F 8C ? ? ? ? E8 ? ? ? ? DC 64 24 28" ).add( 20 ); // 0x14
	auto dw_playerinfo				= xtu::find( "client.dll", "0F BF 80 ? ? ? ? 48" ).add( 0x3 ); // 0x3
	auto dw_gameoverlay_swap		= xtu::find( "gameoverlayrenderer.dll", "FF 15 ? ? ? ? 80 3D ? ? ? ? ? 8B F0 74 0C" ).add( 0x2 );

	xti::g_studiomodel				= dw_studiomodel.at< sdk::engine_studio_api_t* >( );
	xti::g_playermove				= *dw_playermove.at< sdk::player_move_t** >( );
	xti::g_client					= dw_client.at< sdk::c_client_dll_t* >( );
	xti::g_studiomodelrenderer		= dw_studiomodelrenderer.at< sdk::c_studio_model_renderer* >( );
	xti::g_gamespeed				= dw_gamespeed.at< double* >( );
	xti::g_playerextrainfo			= dw_playerinfo.at< std::uintptr_t* >( );
	xti::g_gameoverlayrenderer		= dw_gameoverlay_swap.at< std::uintptr_t >( );

	VirtualProtect( reinterpret_cast< void* >( xti::g_gamespeed ), sizeof( double ), PAGE_EXECUTE_READWRITE, &dw_old_protection );

	xtu::buffered_localise_text_string( "#Menu_Yes" );
	xtu::buffered_localise_text_string( "#Menu_No" );

	auto dw_hookmsg					= xtu::find( "client.dll", "FF 15 ? ? ? ? 83 C4 08 C7 46" ).add( 0x2 );

	xti::g_usermsghook				= dw_hookmsg.at< std::uintptr_t >( );

	xti::g_engine->Con_Printf( "// == // == // == // == // == // == // == // == // == // == //\n" );
};
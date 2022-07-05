#include <windows.h>
#include <string_view>
#include <format>
#include "interfaces.hh"
#include <filesystem>
#include <utils/utils.hh>
//#pragma comment(lib, "winmm.lib")
//#include <../vendor/tamashi/tamashi.hh>

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
std::int32_t xti::g_weaponid = 0;
sdk::c_studio_model_renderer* xti::g_studiomodelrenderer = nullptr;
sdk::client_data_t* xti::g_clientdata = nullptr;
sdk::netchan_t* xti::g_netchan = nullptr;
sdk::c_vote_popup* xti::g_votepopup = nullptr;
sdk::c_default_cvar* xti::g_cvar = nullptr;
sdk::globalvars_t* xti::g_globalvars = nullptr;

double* xti::g_gamespeed = nullptr;
std::uintptr_t* xti::g_playerextrainfo = nullptr;
std::uintptr_t xti::g_gameoverlayrenderer = 0;
std::uintptr_t xti::g_usermsghook = 0;
std::uintptr_t xti::g_gameroverlayrenderer_is_opened = 0;

std::uintptr_t xti::ut_unhook_func = 0;
std::uintptr_t xti::ut_beginread = 0;
std::uintptr_t xti::ut_readstr = 0;
std::uintptr_t xti::ut_readcoord = 0;
std::uintptr_t xti::ut_readshort = 0;
std::uintptr_t xti::ut_readbyte = 0;
std::uintptr_t xti::ut_mapname = 0;
std::uintptr_t xti::ut_buffered = 0;
std::uintptr_t xti::ut_insert_color_change = 0;
std::uintptr_t xti::ut_print_to_console = 0;

// dump the interfaces from the "CreateInterface export" table.
void dump_interfaces( const sdk::interface_reg_t* interface_reg ) {
	for ( auto interface_current = interface_reg; interface_current; interface_current = interface_current->next ) {
		//xti::g_engine->Con_Printf( "[sharingan] module %s\n", interface_current->name );
	};
};

// get the interfaces from the "CreateInterface export" table.
template< typename tty >
tty get_interface( const sdk::interface_reg_t* interface_reg/*const char* from*/, const char* interface_name ) {
	for ( auto interface_current = interface_reg; interface_current; interface_current = interface_current->next ) {
		if ( std::strstr( interface_current->name, interface_name ) ) {
			//xti::g_engine->Con_Printf( "[sharingan] found interface %s, m_createfn: %x\n", interface_current->name, reinterpret_cast< std::uintptr_t >( interface_current->m_createfn( ) ) );
			return reinterpret_cast< tty >( interface_current->m_createfn( ) );
		};
	};
	return NULL;
};


void StatusPlayers( ) {
	for ( int i = 0; i < xti::g_engine->GetMaxClients( ); i++ ) {
		auto entity = xti::g_engine->GetEntityByIndex( i );
		if ( !entity )
			continue;

		if ( !entity->player )
			continue;

		if ( !entity->model )
			continue;

		auto player_info = xti::g_studiomodel->PlayerInfo( entity->index - 1 );
		if ( !player_info || !player_info->name || std::strlen( player_info->name ) < 1 )
			return;

		xti::g_engine->Con_Printf( "player: %s, steamid64: %llu, model: %s\n", player_info->name, player_info->m_nSteamID, player_info->model );
	};
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
	auto dw_studiomodelrenderer		= xtu::find( "client.dll", "B9 ? ? ? ? E8 ? ? ? ? 5F B8" ).add( 0x1 ); // + 0x1
	auto dw_gamespeed				= xtu::find( "hw.dll", "0F 8C ? ? ? ? E8 ? ? ? ? DC 64 24 28" ).add( 20 ); // + 0x14
	auto dw_playerinfo				= xtu::find( "client.dll", "0F BF 80 ? ? ? ? 48" ).add( 0x3 ); // + 0x3
	auto dw_gameoverlay_swap		= xtu::find( "gameoverlayrenderer.dll", "FF 15 ? ? ? ? 80 3D ? ? ? ? ? 8B F0 74 0C" ).add( 0x2 ); // + 0x2
	auto dw_globalvars				= xtu::find( "client.dll", "A1 ? ? ? ? 8B 88 ? ? ? ? 03 48 0C 8D 84 24 ? ? ? ?" ).add( 0x1 ); // + 0x1
	auto dw_gameoverlay_open		= xtu::find( "gameoverlayrenderer.dll", "80 3D ? ? ? ? ? 8B 45 08 74 0A C6 40 09 01" ).add( 0x2 );

	xti::g_studiomodel				= dw_studiomodel.at< sdk::engine_studio_api_t* >( );
	xti::g_playermove				= *dw_playermove.at< sdk::player_move_t** >( );
	xti::g_client					= dw_client.at< sdk::c_client_dll_t* >( );
	xti::g_studiomodelrenderer		= dw_studiomodelrenderer.at< sdk::c_studio_model_renderer* >( );
	xti::g_gamespeed				= dw_gamespeed.at< double* >( );
	xti::g_playerextrainfo			= dw_playerinfo.at< std::uintptr_t* >( );
	xti::g_gameoverlayrenderer		= dw_gameoverlay_swap.at< std::uintptr_t >( );
	xti::g_globalvars				= *dw_globalvars.at< sdk::globalvars_t** >( );
	xti::g_gameroverlayrenderer_is_opened = dw_gameoverlay_open.at< std::uintptr_t >( );

	VirtualProtect( reinterpret_cast< void* >( xti::g_gamespeed ), sizeof( double ), PAGE_EXECUTE_READWRITE, &dw_old_protection );

	//xtu::buffered_localise_text_string( "#Menu_Yes" );
	//xtu::buffered_localise_text_string( "#Menu_No" );

	// i had some plans for the hookmsg, but.. im to lazy and it doesnt really feel necessary to add it since i would only use it for 2 hooks, and that being the vote ones.
	auto dw_hookmsg					= xtu::find( "client.dll", "FF 15 ? ? ? ? 83 C4 08 C7 46" ).add( 0x2 );

	xti::g_usermsghook				= dw_hookmsg.at< std::uintptr_t >( );

	auto dw_unhook_func = xtu::find( "gameoverlayrenderer.dll", "55 8B EC C7" );
	xti::ut_unhook_func = dw_unhook_func.as< std::uintptr_t >( );

	auto dw_beginread = xtu::find( "client.dll", "8B 44 24 08 A3 ? ? ? ? 8B 44" );
	xti::ut_beginread = dw_beginread.as< std::uintptr_t >( );

	auto dw_readstr = xtu::find( "client.dll", "8B 15 ? ? ? ? 33 C0 53" );
	xti::ut_readstr = dw_readstr.as< std::uintptr_t >( );

	auto dw_readcoord = xtu::find( "client.dll", "51 56 8B 35 ? ? ? ? 57 8D 7E 04 3B 3D" );
	xti::ut_readcoord = dw_readcoord.as< std::uintptr_t >( );

	auto dw_readshort = xtu::find( "client.dll", "8B 0D ? ? ? ? 56 8D 71 02" );
	xti::ut_readshort = dw_readshort.as< std::uintptr_t >( );

	auto dw_readbyte = xtu::find( "client.dll", "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E 0E C7 05" ).add( 0x48 );
	xti::ut_readbyte = dw_readbyte.as< std::uintptr_t >( );

	auto dw_mapname = xtu::find( "client.dll", "FF 15 ? ? ? ? 85 C0 74 61 68" );
	xti::ut_mapname = dw_mapname.as< std::uintptr_t >( );

	auto dw_buffered = xtu::find( "client.dll", "53 56 8B 74 24 0C BB ? ? ? ? 57" );
	xti::ut_buffered = dw_buffered.as< std::uintptr_t >( );

	auto dw_insert_color_change = xtu::find( "GameUI.dll", "55 8B EC 8B 91 ? ? ? ? 83 EC 10 8B 45 08 C1 E2 04 03 91" );
	xti::ut_insert_color_change = dw_insert_color_change.as< std::uintptr_t >( );

	auto dw_print_to_console = xtu::find( "GameUI.dll", "55 8B EC 83 EC 08 A1 ? ? ? ? 33 C5 89 45" );
	xti::ut_print_to_console = dw_print_to_console.as< std::uintptr_t >( );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "[hack] " );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "version: %s\n", _CORE_STRING );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ THIS UPDATE ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Added another speed method, called \"fast & slow\"\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) Added multiplier for speedfactor, meaning it now consists of 2 sliders instead of one.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§3) Fixed bug where menu, visuals, crosshair etc.. would render when steam overlay was present.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§4) Removed Fastrun velocity check, it's now uncapped.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§5) Users are now able to rebind their menu key. See \"cfg\" tab.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§6) Users can now disable push notifications. See misc.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§7) Signatures are now stored once, for optimizing reasons.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§8) Users may now move the menu by hovering at the top left corner of the menu.\n\n" );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ NEXT UPDATE ================\n" );
	xtu::console_color_printf( { 228, 89, 89, 255 }, "[PRIORITY!] " );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Fix mouse problems. Some users have reported that cursor freeze at center.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) Rainbow color model. Basically changes the top & bottomcolor of your model and cycles through a hue spectrum.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§3) Freecam.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§4) Spycam. Go to any spot on the map, place a camera and access that camera at any point.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§5) Better vote handling. Do things when vote is against YOU, Important person, etc..\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§6) Change hotkey modes. Being able to select wether an option should be held or toggle'able.\n\n" );
	
	xtu::console_color_printf( { 210, 160, 210, 255 }, "[hack] " );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "config save path: %s\n", ( std::filesystem::current_path( ) / "hack" ).string( ).data( ) );
};
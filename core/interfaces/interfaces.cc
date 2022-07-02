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

	xti::g_studiomodel				= dw_studiomodel.at< sdk::engine_studio_api_t* >( );
	xti::g_playermove				= *dw_playermove.at< sdk::player_move_t** >( );
	xti::g_client					= dw_client.at< sdk::c_client_dll_t* >( );
	xti::g_studiomodelrenderer		= dw_studiomodelrenderer.at< sdk::c_studio_model_renderer* >( );
	xti::g_gamespeed				= dw_gamespeed.at< double* >( );
	xti::g_playerextrainfo			= dw_playerinfo.at< std::uintptr_t* >( );
	xti::g_gameoverlayrenderer		= dw_gameoverlay_swap.at< std::uintptr_t >( );
	xti::g_globalvars				= *dw_globalvars.at< sdk::globalvars_t** >( );

	VirtualProtect( reinterpret_cast< void* >( xti::g_gamespeed ), sizeof( double ), PAGE_EXECUTE_READWRITE, &dw_old_protection );

	//xtu::buffered_localise_text_string( "#Menu_Yes" );
	//xtu::buffered_localise_text_string( "#Menu_No" );

	// i had some plans for the hookmsg, but.. im to lazy and it doesnt really feel necessary to add it since i would only use it for 2 hooks, and that being the vote ones.
	auto dw_hookmsg					= xtu::find( "client.dll", "FF 15 ? ? ? ? 83 C4 08 C7 46" ).add( 0x2 );

	xti::g_usermsghook				= dw_hookmsg.at< std::uintptr_t >( );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "[hack] " );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "version: %s\n", _CORE_STRING );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ NEXT UPDATE ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Fastrun / Fastmove. Making you run faster, using ground strafe mechanics.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) Rainbow color model. Basically changes the top & bottomcolor of your model and cycles through a hue spectrum.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§3) Freecam.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§4) Spycam. Go to any spot on the map, place a camera and access that camera at any point.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§5) Push Notifications. Yes i know, this exists as of now. But in the next you'll be able to turn it off.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§6) Better vote handling. Do things when vote is against YOU, Important person, etc..\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§7) Change hotkey modes. Being able to select wether an option should be held or toggle'able.\n\n" );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ MAYBE IN THE NEXT UPDATE ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Perfect circle strafer. Traces the environment, and proceeds to QUAKE strafe in a circle, avoiding obstacles.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) Better way of handling tickbase manipulation.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§3) Fake ping.\n\n" );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ KNOWN BUGS ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Hotkey's sometimes stop responding to new keybind, meaning you can't rebind. (Working on it)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) There's no way of selecting wether a key should be hold or toggle, (Working on it)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§3) Color picker missing saturation & value sliders (Working on it)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§4) Votekill isn't working / it's disabled. (Yes, im working on fixing it. It had some issues)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§5) Tickbase manipulation stops the user, but flie's forward when autostrafing (Working on a fix for that too.)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§6) WASD AFK Method keeps jumping in water, even when player not afk. (Yes, and i'm working on it, this is not prio.)\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§7) When marking a player important, the text for steamid glitches. This is due to formating issues.\n\n" );

	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ THINGS I WONT ADD ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Console commands. The reason is that there's no way of deleting a command when unloading the cheat.\n   So it will cause a crash.\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§2) Any exploits that's gamebreaking, anything that destroy's anyone else's experience will NOT be added.\n   Such things are not tolerated.\n\n" );
	
	xtu::console_color_printf( { 210, 160, 210, 255 }, "================ THINGS I MAY CONSIDER IN THE FUTURE ================\n" );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "\t§1) Adding a scripting language. Making users able to code their own plugins for this cheat. And\n   being able to load it in at any time. [LUA/CHAISCRIPT]\n\n" );
	
	xtu::console_color_printf( { 210, 160, 210, 255 }, "[hack] " );
	xtu::console_color_printf( { 228, 218, 228, 255 }, "config save path: %s\n", ( std::filesystem::current_path( ) / "hack" ).string( ).data( ) );
	//PlaySoundA( ( char* )tamashi, NULL, SND_MEMORY | SND_SYNC );

	//xti::g_engine->Con_Printf( "// == // == // == // == // == // == // == // == // == // == //\n" );

	//xti::g_console->printf( "hello" );
	//for ( int i = 0; i < 20; i++ ) {
	//	auto ptr = xtu::find( "client.dll", "A1 ? ? ? ? 8B 88 ? ? ? ? 03 48 0C 8D 84 24 ? ? ? ?" ).add( i );
	//	if( auto d = ptr.at< void* >( ) )
	//		xti::g_engine->Con_Printf( "found addr: %X, idx: %i, %04X\n", xtu::get_module_base_address( "client.dll", d ), i, i );
	//};
	// 
	//xti::g_engine->pfnRegisterVariable( "sharingan_kill", "guwi", 0 );
	//xti::g_engine->pfnAddCommand( "sharingan_status", StatusPlayers );
};
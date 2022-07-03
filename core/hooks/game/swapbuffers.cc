#include <windows.h>
#include <hooks/hooks.hh>
#include <renderer/menu.hh>
#include <renderer/renderer.hh>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>
#include <utils/hooking.hh>
#include <hacks/features.hh>
#include <hacks/visuals.hh>
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

WNDPROC old_wnd;
HWND _wnd;

c_mini_hook< xth::fn_swap_buffers > swap_buffer;
void handle_toggle_event( WPARAM wParam ) {
	if ( !options::show_menu ) { // don't even try to capture it if the menu is up.
		if ( Mui::is_key_down( wParam ) ) {
			Mui::set_key_toggle( wParam );
			//Mui::KeysUp[ wParam ] = !Mui::KeysUp[ wParam ];
		};
	};
};

int AppendModules( int idx, const char* module_info, ImColor color = ImColor( 0, 255, 0, 255 ) ) {
	Renderer::AddTextShadow( { 100.0f, 90.0f + idx * 15.0f }, color, module_info, false, Renderer::g_pGilroy, 12.0f );
	return ( idx + 1 );
};

void RenderModules( ) {
	// As of right now, this is the only solution i have. Sure, i could just put every single element that has an hotkey to an array and just
	// iterate through them all, but im lazy and this will have to do it.
	int idx = 0;
	if ( cfg::get< bool >( vars.airstuck ) && Mui::is_key_toggle( cfg::get< int >( vars.airstuck_key ) ) )
		idx = AppendModules( idx, "Airstuck key is ON." );

	if ( cfg::get< bool >( vars.airstuck ) && cfg::get< bool >( vars.airstuck_on_death ) && Mui::is_key_toggle( cfg::get< int >( vars.airstuck_on_death_key ) ) )
		idx = AppendModules( idx, "Airstuck [DEAD] key is ON." );

	if ( cfg::get< bool >( vars.autostrafe ) && Mui::is_key_down( cfg::get< int >( vars.autostrafe_key ) ) )
		idx = AppendModules( idx, "Strafer is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.speedhack ) && Mui::is_key_down( cfg::get< int >( vars.speedhack_key ) ) )
		idx = AppendModules( idx, "Speedhack is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.autocross ) && Mui::is_key_down( cfg::get< int >( vars.autocross_key ) ) )
		idx = AppendModules( idx, "Auto crossbow is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.slowwalk ) && Mui::is_key_down( cfg::get< int >( vars.slowwalk_key ) ) )
		idx = AppendModules( idx, "Slowwalk is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.usespam ) && Mui::is_key_down( cfg::get< int >( vars.usespam_key ) ) )
		idx = AppendModules( idx, "Usespam is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.rev_expl ) && Mui::is_key_down( cfg::get< int >( vars.rev_expl_key ) ) ) {
		idx = AppendModules( idx, "'Magic' is being HELD.", ImColor( 104, 146, 230, 255 ) );
		if( cfg::get< bool >( vars.fakelag ) && cfg::get< int >( vars.rev_expl_key ) == cfg::get< int >( vars.fakelag_key ) )
			idx = AppendModules( idx, "WARNING! 'Magic' glitch wont work if fakelag is ON!", ImColor( 255, 146, 230, 255 ) );
	}

	if ( cfg::get< bool >( vars.dupeglitch ) && Mui::is_key_down( cfg::get< int >( vars.dupeglitch_key ) ) ) {
		idx = AppendModules( idx, "'Magic' Dupe is being HELD.", ImColor( 104, 146, 230, 255 ) );
		if ( cfg::get< bool >( vars.fakelag ) && cfg::get< int >( vars.rev_expl_key ) == cfg::get< int >( vars.fakelag_key ) )
			idx = AppendModules( idx, "WARNING! 'Magic' Dupe glitch wont work if fakelag is ON!", ImColor( 255, 146, 230, 255 ) );
	}

	if ( cfg::get< bool >( vars.mirror ) && Mui::is_key_down( cfg::get< int >( vars.mirror_key ) ) )
		idx = AppendModules( idx, "Mirror is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.movement ) && Mui::is_key_down( cfg::get< int >( vars.movement_record ) ) )
		idx = AppendModules( idx, "Movement [RECORD] is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.movement ) && Mui::is_key_down( cfg::get< int >( vars.movement_play ) ) )
		idx = AppendModules( idx, "Movement [PLAY] is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.vote ) && Mui::is_key_down( cfg::get< int >( vars.vote_key ) ) )
		idx = AppendModules( idx, "Vote is being HELD.", ImColor( 255, 150, 20, 255 ) );

	if ( cfg::get< bool >( vars.vote_kill ) && Mui::is_key_toggle( cfg::get< int >( vars.vote_kill_key ) ) )
		idx = AppendModules( idx, "Votekill key is ON." );
};

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall hk_wnd_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {
	switch ( uMsg ) {
	case WM_MOUSEWHEEL:break;
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK: {
		Mui::set_key_down( VK_RBUTTON );
		//Mui::Keys[ VK_RBUTTON ] = true;
		handle_toggle_event( VK_RBUTTON );
	}break;
	case WM_RBUTTONUP:
		//Mui::Keys[ VK_RBUTTON ] = false;
		Mui::reset_key_down( VK_RBUTTON );
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK: {
		Mui::set_key_down( VK_MBUTTON );
		//Mui::Keys[ VK_MBUTTON ] = true;
		handle_toggle_event( VK_MBUTTON );
	}break;
	case WM_MBUTTONUP:
		Mui::reset_key_down( VK_MBUTTON );
		//Mui::Keys[ VK_MBUTTON ] = false;
		break;
	case WM_XBUTTONDOWN:
	{
		auto xbtn = GET_XBUTTON_WPARAM( wParam );
		if ( xbtn == XBUTTON1 ) {
			//Mui::Keys[ VK_XBUTTON1 ] = true;
			Mui::set_key_down( VK_XBUTTON1 );
			handle_toggle_event( VK_XBUTTON1 );
		}
		else if ( xbtn == XBUTTON2 ) { 
			Mui::set_key_down( VK_XBUTTON2 );
			//Mui::Keys[ VK_XBUTTON2 ] = true; 
			handle_toggle_event( VK_XBUTTON2 );
		}
	}break;
	case WM_XBUTTONUP:
	{
		auto xbtn = GET_XBUTTON_WPARAM( wParam );
		if ( xbtn == XBUTTON1 )
			Mui::reset_key_down( VK_XBUTTON1 );//Mui::Keys[ VK_XBUTTON1 ] = false;
		else if ( xbtn == XBUTTON2 )
			Mui::reset_key_down( VK_XBUTTON2 );//Mui::Keys[ VK_XBUTTON2 ] = false;
	}
	break;
	case WM_KEYDOWN:
	{
		Mui::set_key_down( wParam );//Mui::Keys[ wParam ] = true;
		handle_toggle_event( wParam );
	}break;
	case WM_KEYUP:
	{
		Mui::reset_key_down( wParam );//Mui::Keys[ wParam ] = false; 
	}break;
	default:
		break;
	};

	if ( Mui::is_key_down( cfg::get< int >( vars.menu_key ) ) )
		options::show_menu = !options::show_menu;

	if ( options::show_menu ) {
		ImGui_ImplWin32_WndProcHandler( hWnd, uMsg, wParam, lParam );
		return true;
	};

	return CallWindowProcA( old_wnd, hWnd, uMsg, wParam, lParam );
};

bool __stdcall xth::hk_swap_buffers( HDC hdc ) {
	static bool g_initialize_once = false;
	static HWND g_wnd_from_hdc = nullptr;

	if ( !g_initialize_once ) {
		g_wnd_from_hdc = WindowFromDC( hdc );
		ImGui::CreateContext( );

		Mui::Setup( );

		_wnd = g_wnd_from_hdc;
		old_wnd = reinterpret_cast< WNDPROC >( SetWindowLongPtrA( _wnd, GWLP_WNDPROC, reinterpret_cast< LONG_PTR >( hk_wnd_proc ) ) );
		ImGui_ImplWin32_Init( g_wnd_from_hdc );
		ImGui_ImplOpenGL3_Init( );

		Renderer::PushNotification( "Press '", "INSERT", "' to open the menu.", 4000.0f );
		Renderer::PushNotification( "Open '", "CONSOLE", "' to see the latest news.", 7000.0f, ImColor( 242, 196, 102, 255 ) );

		g_initialize_once = true;
	};
	ImGui_ImplWin32_NewFrame( );
	ImGui_ImplOpenGL3_NewFrame( );
	ImGui::NewFrame( );

	if ( !xtu::is_overlay_opened( ) ) {
		Renderer::Setup( ImGui::GetBackgroundDrawList( ) );
		visuals::on_render( );

		Renderer::RenderNotifications( );

		RenderModules( );
		Mui::Render( );
	};

	ImGui::EndFrame( );
	ImGui::Render( );

	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );
	return swap_buffer.get( )( hdc );
};

void xth::swapbuffers::glhooks( ) {
};

void xth::swapbuffers::hook( ) {
	swap_buffer = c_mini_hook< xth::fn_swap_buffers >( xti::g_gameoverlayrenderer );
	swap_buffer.hook( hk_swap_buffers );
};

void xth::swapbuffers::unhook( ) {
	SetWindowLongPtrA( _wnd, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( old_wnd ) );
	swap_buffer.unhook( );
};
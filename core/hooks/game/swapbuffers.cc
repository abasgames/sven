#include <windows.h>
#include <hooks/hooks.hh>
#include <hacks/visuals.hh>
#include <renderer/menu.hh>
#include <renderer/renderer.hh>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_win32.h>
#include <utils/hooking.hh>
#include <hacks/features.hh>
#include <gl/GL.h>
#pragma comment(lib, "opengl32.lib")

#define _CORE_VERSION "0.4.7"
#define _CORE_STRING "svencoop - " _CORE_VERSION
#define _CORE_BUILD_DATE __TIME__ " @ " __DATE__

WNDPROC old_wnd;
HWND _wnd;

c_mini_hook< xth::fn_swap_buffers > swap_buffer;

extern LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
LRESULT __stdcall hk_wnd_proc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

	switch ( uMsg ) {
	case WM_RBUTTONDOWN:
	case WM_RBUTTONDBLCLK:
		Mui::Keys[ VK_RBUTTON ] = true;
		break;
	case WM_RBUTTONUP:
		Mui::Keys[ VK_RBUTTON ] = false;
		break;
	case WM_MBUTTONDOWN:
	case WM_MBUTTONDBLCLK:
		Mui::Keys[ VK_MBUTTON ] = true;
		break;
	case WM_MBUTTONUP:
		Mui::Keys[ VK_MBUTTON ] = false;
		break;
	case WM_XBUTTONDOWN:
	{
		auto xbtn = GET_XBUTTON_WPARAM( wParam );
		if ( xbtn == XBUTTON1 )
			Mui::Keys[ VK_XBUTTON1 ] = true;
		else if ( xbtn == XBUTTON2 )
			Mui::Keys[ VK_XBUTTON2 ] = true;
	}
	break;
	case WM_XBUTTONUP:
	{
		auto xbtn = GET_XBUTTON_WPARAM( wParam );
		if ( xbtn == XBUTTON1 )
			Mui::Keys[ VK_XBUTTON1 ] = false;
		else if ( xbtn == XBUTTON2 )
			Mui::Keys[ VK_XBUTTON2 ] = false;
	}
	break;
	case WM_KEYDOWN:
		Mui::Keys[ wParam ] = true;
		break;
	case WM_KEYUP:
		Mui::Keys[ wParam ] = false;
		break;
	default:
		break;
	};

	if ( Mui::Keys[ options::show_menu_key ] )
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

		g_initialize_once = true;
	};
	ImGui_ImplWin32_NewFrame( );
	ImGui_ImplOpenGL3_NewFrame( );
	ImGui::NewFrame( );

	Renderer::Setup( ImGui::GetBackgroundDrawList( ) );

	Renderer::AddText( ImVec2( 10.0f, 10.0f ), ImColor( 255, 255, 255, 255 ), _CORE_STRING );
	Renderer::AddText( ImVec2( 10.0f, 20.0f ), ImColor( 255, 255, 255, 255 ), _CORE_BUILD_DATE );
	Renderer::AddText( ImVec2( 10.0f, 30.0f ), ImColor( 255, 255, 255, 255 ), xti::g_rungameengine->is_in_game( ) ? "ingame" : "not ingame" );

	visuals::on_visuals_esp( );
	Mui::Render( );

	ImGui::EndFrame( );
	ImGui::Render( );

	ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );
	return swap_buffer.get( )( hdc );
};

void xth::swapbuffers::hook( ) {
	swap_buffer = c_mini_hook< xth::fn_swap_buffers >( xti::g_gameoverlayrenderer );
	swap_buffer.hook( hk_swap_buffers );
};

void xth::swapbuffers::unhook( ) {
	SetWindowLongPtrA( _wnd, GWL_WNDPROC, reinterpret_cast< LONG_PTR >( old_wnd ) );
	swap_buffer.unhook( );
};
#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <hacks/vars.hh>
#include <interfaces/interfaces.hh>
#include <renderer/renderer.hh>
#include <format>

/*
* this is probably the one source file i've been working the most on.
* 
*/

#define AddRuntimeHook( def, index, function, original ) \
const auto m_pfnSharinganPtr ## def = reinterpret_cast< void* >( sdk::vtable( vote, index ) ); \
xth::add( m_pfnSharinganPtr ## def, &function, reinterpret_cast< void** >( &original ) ); \
xth::enable( m_pfnSharinganPtr ## def )

xth::fn_votepopup_msgfunc org_votepopup_msgfunc = nullptr;
xth::fn_votepopup_update org_votepopup_update = nullptr;
xth::fn_votepopup_reset org_votepopup_reset = nullptr;
xth::fn_end_vote org_end_vote = nullptr;
xth::fn_start_vote org_start_vote = nullptr;
xth::fn_votepopup_open org_votepopup_open = nullptr;
xth::fn_votepopup_close org_votepopup_close = nullptr;

void* __fastcall xth::hk_votepopup_reset( void* ecx, void* edx ) {
	return org_votepopup_reset( ecx );
};

void __fastcall xth::hk_votepopup_update( void* ecx, void* edx ) {
	org_votepopup_update( ecx );
}

int __fastcall xth::hk_votepopup_open( void* ecx, void* edx ) {
	auto vote = reinterpret_cast< sdk::c_vote_popup* >( ecx );
	if ( cfg::get< bool >( vars.vote ) && cfg::get< bool >( vars.vote_auto ) )
		vote->slot_input( cfg::get< int >( vars.vote_mode ) == 0 ? 1 : 0 );
	return org_votepopup_open( ecx );
};

int __fastcall xth::hk_votepopup_close( void* ecx, void* edx ) {
	auto vote = reinterpret_cast< sdk::c_vote_popup* >( ecx );
	return org_votepopup_close( ecx );
};

int xth::hk_end_vote( const char* message, int size, void* buffer ) {
	Renderer::PushNotification( "The ", "vote", " has ended.", 4000.0f, ImColor( 112, 241, 144, 255 ) );
	return org_end_vote( message, size, buffer );
};

int xth::hk_start_vote( const char* message, int size, void* buffer ) {
	xtu::begin_read( buffer, size );
	char* sz_vote_message = xtu::read_string( );

	Renderer::PushNotification( "A ", "vote", " has been called!", 4000.0f, ImColor( 232, 184, 73, 255 ) );
	return cfg::get< bool >( vars.vote ) && cfg::get< bool >( vars.vote_disable ) ? 1 : org_start_vote( message, size, buffer );
};

int __fastcall xth::hk_votepopup_msgfunc( sdk::c_vote_popup* vote, void* edx, const char* panel_name, int size, void* data ) {
	static bool g_initialised = false;
	if ( !g_initialised ) {
		// initialize a "runtime hooking" schedule.

		AddRuntimeHook( m_reset, 127, hk_votepopup_reset, org_votepopup_reset );
		Renderer::PushNotification( "Hook: ", "Reset", " has been hooked to the game.", 4000.0f, ImColor( 232, 184, 73, 255 ) );

		AddRuntimeHook( m_open, 128, hk_votepopup_open, org_votepopup_open );
		Renderer::PushNotification( "Hook: ", "Open", " has been hooked to the game.", 4000.0f, ImColor( 241, 112, 235, 255 ) );

		AddRuntimeHook( m_close, 129, hk_votepopup_close, org_votepopup_close );
		Renderer::PushNotification( "Hook: ", "Close", " has been hooked to the game.", 4000.0f, ImColor( 112, 222, 241, 255 ) );

		AddRuntimeHook( m_update, 132, hk_votepopup_update, org_votepopup_update );
		Renderer::PushNotification( "Hook: ", "Update", " has been hooked to the game.", 4000.0f, ImColor( 62, 180, 186, 255 ) );

		// This will hook the following.
		// CVotePopup::Reset
		// CVotePopup::Open
		// CVotePopup::Close
		// CVotePopup::Update

		g_initialised = true;
	};

	xtu::begin_read( data, size );

	if ( cfg::get< bool >( vars.autohomie ) ) {
		
	};
	org_votepopup_msgfunc( vote, panel_name, size, data );
	return 1;
};

void xth::votemenu::hook( ) {
	const auto votepopup_msgfunc = xtu::find( "client.dll", "56 57 FF 74 24 10 8B F1 FF 74 24 18 E8 ? ? ? ? E8 ? ? ? ? 89" ); // if you're looking for just closing the popup, then you could technically just hook "VoteMenu" and return it.
	
	auto votepopup_start = xtu::find( "client.dll", "8B 0D ? ? ? ? 85 C9 74 11 FF 74 24 0C" );

	auto votepopup_end = votepopup_start.add( 0x30 );

	// CVotePopup::MsgFunc_VoteMenu
	xth::add( votepopup_msgfunc.as< void* >( ), &hk_votepopup_msgfunc, reinterpret_cast< void** >( &org_votepopup_msgfunc ) );

	// UserMsgHook::VoteMenu
	xth::add( votepopup_start.as< void* >( ), &hk_start_vote, reinterpret_cast< void** >( &org_start_vote ) );

	// UserMsgHook::EndVote
	xth::add( votepopup_end.as< void* >( ), &hk_end_vote, reinterpret_cast< void** >( &org_end_vote ) );
};
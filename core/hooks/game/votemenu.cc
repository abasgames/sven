#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>

xth::fn_votepopup_msgfunc org_votepopup_msgfunc = nullptr;

/// <summary>
/// Så fort det kommer en vote om att döda/sparka/byta mapp, så blir denna funktionen kallad.
/// </summary>
/// <param name="vote">thisptr - ecx</param>
/// <param name="edx">edx</param>
/// <param name="panel_name">a1</param>
/// <param name="size">a2</param>
/// <param name="data">a3</param>
/// <returns>original</returns>
 
int __fastcall xth::hk_votepopup_msgfunc( sdk::c_vote_popup* vote, void* edx, const char* panel_name, int size, void* data ) {
	static bool g_initialised = false;
	if ( !g_initialised ) {
		// tdl; göra "runtime hooking". menas att den ska hookas 1 gång under runtime, detta är för att vi inte kan få tag i
		// pointern till CVotePopup, så det vi kan göra är att hooka "vote->open()/vote->close()", eftersom ecx håller i pointern redan.
		// så hooka sdk::vtable( vote, 128 ) / sdk::vtable( vote, 129 ).
		// i den hooken, så ska du köra en vote->slot_input( 0 ) för att rösta nej och vote->slot_input( 1 ) för att rösta ja.
		g_initialised = true;
	};

	return org_votepopup_msgfunc( vote, panel_name, size, data );
};

void xth::votemenu::hook( ) {
	const auto votepopup_msgfunc = xtu::find( "client.dll", "56 57 FF 74 24 10 8B F1 FF 74 24 18 E8 ? ? ? ? E8 ? ? ? ? 89" ); // if you're looking for just closing the popup, then you could technically just hook "VoteMenu" and return it.
	xth::add( votepopup_msgfunc.as< void* >( ), &hk_votepopup_msgfunc, reinterpret_cast< void** >( &org_votepopup_msgfunc ) );
}
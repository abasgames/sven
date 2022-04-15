#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>

xth::fn_votepopup_msgfunc org_votepopup_msgfunc = nullptr;

/// <summary>
/// S� fort det kommer en vote om att d�da/sparka/byta mapp, s� blir denna funktionen kallad.
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
		// tdl; g�ra "runtime hooking". menas att den ska hookas 1 g�ng under runtime, detta �r f�r att vi inte kan f� tag i
		// pointern till CVotePopup, s� det vi kan g�ra �r att hooka "vote->open()/vote->close()", eftersom ecx h�ller i pointern redan.
		// s� hooka sdk::vtable( vote, 128 ) / sdk::vtable( vote, 129 ).
		// i den hooken, s� ska du k�ra en vote->slot_input( 0 ) f�r att r�sta nej och vote->slot_input( 1 ) f�r att r�sta ja.
		g_initialised = true;
	};

	return org_votepopup_msgfunc( vote, panel_name, size, data );
};

void xth::votemenu::hook( ) {
	const auto votepopup_msgfunc = xtu::find( "client.dll", "56 57 FF 74 24 10 8B F1 FF 74 24 18 E8 ? ? ? ? E8 ? ? ? ? 89" ); // if you're looking for just closing the popup, then you could technically just hook "VoteMenu" and return it.
	xth::add( votepopup_msgfunc.as< void* >( ), &hk_votepopup_msgfunc, reinterpret_cast< void** >( &org_votepopup_msgfunc ) );
}
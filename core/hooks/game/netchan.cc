#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <hacks/vars.hh>
#include <interfaces/interfaces.hh>
#include <utils/hash.hh>

xth::fn_netchan_can_packet org_netchan_can_packet = nullptr;
xth::fn_netchan_transmit org_netchan_transmit = nullptr;
xth::fn_netchan_process org_netchan_process = nullptr;

bool FreezeOnDeath(sdk::netchan_t* netchan, int length, byte* data ) {
	if ( !cfg::get< bool >( vars.airstuck ) )
		return false;

	if ( !cfg::get< bool >( vars.airstuck_on_death ) )
		return false;

	if ( !Mui::is_key_toggle( cfg::get< int >( vars.airstuck_on_death_key ) ) )
		return false;

	if ( !xtu::is_player_dead( ) )
		return false;

	org_netchan_transmit( netchan, 0, nullptr );
	return true;
};

static float m_nTickCount = 0.0f;
static bool b_update_antiafk = false;
static bool b_in_motion = false;
bool FreezeOnAlive( sdk::netchan_t* netchan, int length, byte* data ) {
	if ( !cfg::get< bool >( vars.airstuck ) )
		return false;

	if ( !Mui::is_key_toggle( cfg::get< int >( vars.airstuck_key ) ) )
		return false;

	if ( xtu::is_player_dead( ) && cfg::get< bool >( vars.airstuck_on_death ) )
		return false;

	if ( cfg::get< bool >( vars.airstuck_air_afk ) && !( xti::g_playermove->flags & FL_ONGROUND ) ) {
		if ( !b_in_motion ) {
			if ( GetTickCount64( ) > m_nTickCount ) {
				m_nTickCount = GetTickCount64( ) + ( 1000.0f * 15.0f );
				b_in_motion = true;
			};
		};
	};

	if ( b_in_motion ) {
		b_in_motion = false;
		return false;
	};

	if ( !b_in_motion ) {
		switch ( cfg::get< int >( vars.airstuck_mode ) ) {
		case options::airstuck_modes::airstuck_freeze:
		case options::airstuck_modes::airstuck_complete_freeze: {
			org_netchan_transmit( netchan, 0, nullptr );
			return true;
		};
		default:
			return false;
		};
	};

	return false;
};

void xth::hk_netchan_transmit( sdk::netchan_t* netchan, int length, byte* data ) {
	if ( FreezeOnDeath( netchan, length, data ) || FreezeOnAlive( netchan, length, data ) )
		return;

	org_netchan_transmit( netchan, length, data );
};

int xth::hk_netchan_can_packet( sdk::netchan_t* chan ) {
	if ( !xtu::send_packet )
		return 0;
	return org_netchan_can_packet( chan );
};

bool xth::hk_netchan_process( sdk::netchan_t* chan ) {
	if ( cfg::get< bool >( vars.airstuck ) )
		if ( Mui::is_key_toggle( cfg::get< int >( vars.airstuck_key ) ) && cfg::get< int >( vars.airstuck_mode ) == options::airstuck_modes::airstuck_process )
			return false;

	return org_netchan_process( chan );
};


void xth::netchan::hook( ) {
	// Netchan_Transmit
	const auto netchan_transmit = xtu::find( "hw.dll", "B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 8B 84 24 ? ? ? ? 53 55" );

	// Netchan_CanPacket
	const auto netchan_canpacket = xtu::find( "hw.dll", "8B 44 24 04 7A 15 83 78 04 01" ).sub( 0xf );

	// Netchan_Process
	const auto netchan_process = xtu::find( "hw.dll", "83 EC 34 83 3D ? ? ? ? ? 53" );

	xth::add( netchan_transmit.as< void* >( ), &hk_netchan_transmit, reinterpret_cast< void** >( &org_netchan_transmit ) );
	xth::add( netchan_canpacket.as< void* >( ), &hk_netchan_can_packet, reinterpret_cast< void** >( &org_netchan_can_packet ) );
	xth::add( netchan_process.as< void* >( ), &hk_netchan_process, reinterpret_cast< void** >( &org_netchan_process ) );
};
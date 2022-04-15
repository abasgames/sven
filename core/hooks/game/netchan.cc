#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>

xth::fn_net_can_packet org_net_can_packet = nullptr;
xth::fn_net_chan_transmit org_net_chan_transmit = nullptr;

void xth::hk_netchan_transmit( sdk::netchan_t* netchan, int length, byte* data ) {
	if ( options::general::airstuck ) {
		if ( Mui::Keys[ options::general::airstuck_key ] ) {
			org_net_chan_transmit( netchan, 0, nullptr ); // cancel size and data
			return;
		}
	};

	org_net_chan_transmit( netchan, length, data );
};

int xth::hk_net_can_packet( sdk::netchan_t* chan ) {
	if ( !xtu::send_packet )
		return 0;
	return org_net_can_packet( chan );
};

void xth::netchan::hook( ) {
	const auto netchan_transmit = xtu::find( "hw.dll", "B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 33 C4 89 84 24 ? ? ? ? 8B 84 24 ? ? ? ? 53 55" );
	const auto netchan_canpacket = xtu::find( "hw.dll", "8B 44 24 04 7A 15 83 78 04 01" ).sub( 0xf );

	xth::add( netchan_transmit.as< void* >( ), &hk_netchan_transmit, reinterpret_cast< void** >( &org_net_chan_transmit ) );
	xth::add( netchan_canpacket.as< void* >( ), &hk_net_can_packet, reinterpret_cast< void** >( &org_net_can_packet ) );
};
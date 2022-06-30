#define _USE_MATH_DEFINES
#include <windows.h>
#include <cmath>
#include <cstdint>
#include <interfaces/interfaces.hh>
#include "utils.hh"

#define in_range(x,a,b)    (x >= a && x <= b)
#define get_bits( x )    (in_range((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (in_range(x,'0','9') ? x - '0' : 0))
#define get_byte( x )    (get_bits(x[0]) << 4 | get_bits(x[1]))

bool xtu::send_packet = true;

float xtu::viewangles_x = 0.0f;
float xtu::viewangles_y = 0.0f;

std::uintptr_t xtu::find_pattern( const std::string_view& from, const std::string_view& pattern ) noexcept {
	const auto base = get_module_info( from );
	const auto base_start = base.first;
	const auto base_end = base_start + base.second;
	auto base_pattern = pattern.data( );

	std::uintptr_t matches = 0;
	for ( auto current = base_start; current < base_end; current++ ) {
		if ( !*base_pattern )
			return matches;

		if ( *reinterpret_cast< const unsigned char* >( base_pattern ) == 63 || *reinterpret_cast< unsigned char* >( current ) == get_byte( base_pattern ) ) {
			if ( !matches )
				matches = current;

			if ( !base_pattern[ 2 ] )
				return matches;

			*reinterpret_cast< const unsigned char* >( base_pattern ) == 63 ? base_pattern += 2 : base_pattern += 3;
		}
		else {
			base_pattern = pattern.data( );
			matches = 0;
		};
	};
	return 0;
};

std::uintptr_t xtu::find_string( const std::string_view& from, const std::string_view& string ) noexcept {
	auto m_pattern = string.data( );
	auto scanned_module = get_module_info( from );
	std::uintptr_t range_end = scanned_module.first + scanned_module.second;

	std::size_t pattern_length = strlen( string.data( ) );
	bool found = false;

	for ( std::uintptr_t current = scanned_module.first; current < range_end - pattern_length; current++ ) {
		found = true;
		for ( std::size_t idx = 0; idx < pattern_length; idx++ ) {
			if ( m_pattern[ idx ] != *( PCHAR )( current + idx ) ) {
				found = false;
				break;
			};
		};

		if ( found )
			return current + 0;
	};
	return 0;
}
address xtu::find( const std::string_view& from, const std::string_view& pattern ) noexcept {
	return address{ find_pattern( from, pattern ) };
};

std::uintptr_t xtu::get_module( const char* sz_module ) {
	const auto handle = reinterpret_cast< std::uintptr_t >( GetModuleHandleA( sz_module ) );
	if ( !handle )
		return 0;

	auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( handle );
	if ( !dos_header )
		return 0;

	auto nt_header = reinterpret_cast< PIMAGE_NT_HEADERS >( handle + dos_header->e_lfanew );
	if ( !nt_header )
		return 0;

	return static_cast< std::uintptr_t >( nt_header->OptionalHeader.ImageBase );
};

std::pair<std::uintptr_t, std::uintptr_t> xtu::get_module_info( const std::string_view& from ) {
	const auto handle = reinterpret_cast< std::uintptr_t >( GetModuleHandleA( from.data( ) ) );
	if ( !handle )
		return { 0, 0 };

	auto dos_header = reinterpret_cast< PIMAGE_DOS_HEADER >( handle );
	if ( !dos_header )
		return { 0, 0 };

	auto nt_header = reinterpret_cast< PIMAGE_NT_HEADERS >( handle + dos_header->e_lfanew );
	if ( !nt_header )
		return { 0, 0 };

	return std::make_pair( nt_header->OptionalHeader.ImageBase, nt_header->OptionalHeader.SizeOfImage );
};

std::uintptr_t xtu::get_module_addr( const char* sz_module, const std::uintptr_t addr ) {
	return reinterpret_cast< std::uintptr_t >( GetModuleHandleA( sz_module ) ) + addr;
};

std::uintptr_t xtu::get_module_base_address( const char* sz_module, void* ptr ) {
	auto base = get_module( sz_module );
	return reinterpret_cast< std::uintptr_t >( ptr ) - base;
};

double xtu::deg2rad( double deg ) {
	return deg * M_PI / 180.0;
};

double xtu::rad2deg( double angle ) {
	return angle * 180.0 / M_PI;
};

float xtu::deg2radf( float deg ) {
	return deg * 3.14159265358979323846f / 180.0f;
};

float xtu::rad2degf( float angle ) {
	return angle * 180.0f / 3.14159265358979323846f;
};

void xtu::vector_angles( const sdk::c_vector& forward, sdk::c_vector& angles ) {
	float tmp, yaw, pitch;
	if ( forward[ 1 ] == 0 && forward[ 0 ] == 0 ) {
		yaw = 0;
		if ( forward[ 2 ] > 0 )
			pitch = 270;
		else
			pitch = 90;
	}
	else {
		yaw = static_cast< float >( ( std::atan2( forward[ 1 ], forward[ 0 ] ) * 180.f / M_PI ) );
		if ( yaw < 0 )
			yaw += 360;

		tmp = std::sqrtf( forward[ 0 ] * forward[ 0 ] + forward[ 1 ] * forward[ 1 ] );
		pitch = static_cast< float >( ( std::atan2( -forward[ 2 ], tmp ) * 180.f / M_PI ) );
		if ( pitch < 0 )
			pitch += 360;
	};
	angles[ 0 ] = pitch;
	angles[ 1 ] = yaw;
	angles[ 2 ] = 0;
}
void xtu::angle_vectors( const sdk::c_vector& angles, sdk::c_vector* forward )
{
	float sp, sy, cp, cy;

	sy = std::sinf( deg2radf( angles[ 1 ] ) );
	cy = std::cosf( deg2radf( angles[ 1 ] ) );
	sp = std::sinf( deg2radf( angles[ 0 ] ) );
	cp = std::cosf( deg2radf( angles[ 0 ] ) );

	forward->x = cp * cy;
	forward->y = cp * sy;
	forward->z = -sp;
};

bool xtu::world_to_screen( sdk::c_vector in, sdk::c_vector& out ) {
	const auto screen_center_x = static_cast< int >( GetSystemMetrics( SM_CXSCREEN ) / 2 );
	const auto screen_center_y = static_cast< int >( GetSystemMetrics( SM_CYSCREEN ) / 2 );

	sdk::c_vector _out;
	auto rasterizer = xti::g_engine->pTriAPI->WorldToScreen( in, _out );
	if ( _out[ 0 ] < 1 && _out[ 1 ] < 1 && _out[ 0 ] > -1 && _out[ 1 ] > -1 && !rasterizer ) {
		out.x = _out[ 0 ] * screen_center_x + screen_center_x;
		out.y = -_out[ 1 ] * screen_center_y + screen_center_y;
		return true;
	};

	return false;
};

void xtu::fix_movement( sdk::c_vector& base_angles, sdk::c_user_cmd* cmd ) {
	sdk::c_vector vec_move( cmd->forwardmove, cmd->sidemove, cmd->upmove );
	float speed = std::sqrt( vec_move.x * vec_move.x + vec_move.y * vec_move.y );

	sdk::c_vector ang_move;
	xtu::vector_angles( vec_move, ang_move );

	float yaw = static_cast< float >( deg2rad( cmd->viewangles.y - base_angles.y + ang_move.y ) );

	if ( cmd->viewangles.x > 89 || cmd->viewangles.x < -89 )
		cmd->forwardmove = -std::cos( yaw ) * speed;
	else
		cmd->forwardmove = std::cos( yaw ) * speed;
	cmd->sidemove = std::sin( yaw ) * speed;
};

// asked swift if he had an idea on how i should get the cursor pos, and he told me to unhook their functions.
// so credits to him for that.
void xtu::overlay_unhook_function( void* func, bool rehook ) { 
	// Unhook
	auto dw_unhook_func = xtu::find( "gameoverlayrenderer.dll", "55 8B EC C7" );

	using fn = void( * )( void* );
	static auto ofn = dw_unhook_func.as< fn >( );
	if ( !ofn )
		return;

	ofn( func );
};

void xtu::begin_read( void* buffer, int size ) {
	// BEGIN_READ
	auto dw_beginread = xtu::find( "client.dll", "8B 44 24 08 A3 ? ? ? ? 8B 44" );

	using fn = void( * )( void*, int );
	static auto ofn = dw_beginread.as< fn >( );
	if ( !ofn )
		return;

	ofn( buffer, size );
};

char* xtu::read_string( ) {
	// READ_STRING
	auto dw_readstr = xtu::find( "client.dll", "8B 15 ? ? ? ? 33 C0 53" );

	using fn = char* ( * )( );
	static auto ofn = dw_readstr.as< fn >( );
	if ( !ofn )
		return nullptr;

	return ofn( );
};

double xtu::read_coord( ) {
	// READ_COORD
	auto dw_readcoord = xtu::find( "client.dll", "51 56 8B 35 ? ? ? ? 57 8D 7E 04 3B 3D" );

	using fn = double( * )( );
	static auto ofn = dw_readcoord.as< fn >( );
	if ( !ofn )
		return 0.0;

	return ofn( );
};

unsigned short xtu::read_short( ) {
	// READ_SHORT
	auto dw_readshort = xtu::find( "client.dll", "8B 0D ? ? ? ? 56 8D 71 02" );

	using fn = unsigned short( * )( );
	static auto ofn = dw_readshort.as< fn >( );
	if ( !ofn )
		return 0;

	return ofn( );
};

void xtu::usermsg( const char* message, void* func ) {
	address usr{ xti::g_usermsghook };

	using fn = void( * )( const char*, void* );
	static auto ofn = ( fn )( usr.at< std::uintptr_t >( ) );
	if ( !ofn )
		return;

	ofn( message, func );
};

// yeah so this WAS planned.
void* xtu::usermsg_org( const char* message ) {
	return nullptr;
};

BYTE xtu::read_byte( ) {
	// READ_BYTE
	auto dw_readbyte = xtu::find( "client.dll", "8B 0D ? ? ? ? 8D 51 01 3B 15 ? ? ? ? 7E 0E C7 05" );

	using fn = BYTE( * )( );
	static auto ofn = dw_readbyte.add( 0x48 ).as< fn >( );
	if ( !ofn )
		return 0x00;

	return ofn( );
}
char* xtu::util_get_map_name( )
{
	// UTIL_GetMapName
	auto dw_mapname = xtu::find( "client.dll", "FF 15 ? ? ? ? 85 C0 74 61 68" );
	using fn = char* ( * )( );
	static auto ofn = dw_mapname.as< fn >( );
	if ( !ofn )
		return nullptr;

	return ofn( );
}
const char* xtu::get_name_changed( int index ) // yeah, im from the csgo community so you knew i had to do it.
{
	switch ( index ) {
	case 0: return "     ";
	case 1: return "    g";
	case 2: return "   gu";
	case 3: return "  guw";
	case 4: return " guwi";
	case 5: return "guwi ";
	case 6: return "uwi  ";
	case 7: return "wi   ";
	case 8: return "i    ";
	case 9: return "     ";
	};
	return "unknown";
}
const char* xtu::get_steam_name( )
{
	return sdk::instanced< const char* >( "steam_api.dll", "SteamAPI_ISteamFriends_GetPersonaName" );
};

void xtu::buffered_localise_text_string( const char* msg ) {
	auto dw_buffered = xtu::find( "client.dll", "53 56 8B 74 24 0C BB ? ? ? ? 57" );

	using ofn = char* ( * )( const char* );
	static auto org = dw_buffered.as< ofn >( );
	if ( !org )
		return;

	xti::g_engine->Con_Printf( "[sharingan] sent %s -> 0x%X, got [%s]\n", msg, org( msg ), org( msg ) );
};

int xtu::insert_color_change( DWORD* ecx, int a2 ) // 55 8B EC 8B 91 ? ? ? ? 83 EC 10 8B 45 08 C1 E2 04 03 91
{
	auto dw_insert_color_change = xtu::find( "GameUI.dll", "55 8B EC 8B 91 ? ? ? ? 83 EC 10 8B 45 08 C1 E2 04 03 91" );

	using fn = int( __thiscall* )( DWORD*, int );
	static auto ofn = dw_insert_color_change.as< fn >( );
	if ( !ofn )
		return -1;

	return ofn( ecx, a2 );
};

int xtu::insert_text( void* ecx, const char* msg ) // 55 8B EC 83 EC 08 A1 ? ? ? ? 33 C5 89 45
{
	auto dw_print_to_console = xtu::find( "GameUI.dll", "55 8B EC 83 EC 08 A1 ? ? ? ? 33 C5 89 45" );

	using fn = int( __thiscall* )( void*, const char* );
	static auto ofn = dw_print_to_console.as< fn >( );
	if ( !ofn )
		return -1;

	return ofn( ecx, msg );
};

int xtu::color_print( DWORD* thisptr, const char* msg, sdk::c_color32 color ) {
	insert_color_change( ( DWORD* )thisptr[ 0x46 ], color.hex_abgr( ) );
	return insert_text( ( void* )thisptr[ 0x46 ], ( const char* )msg );
}

void xtu::console_color_printf( sdk::c_color32 color, const char* msg, ... ) {
	char buff[ 256 ];
	va_list list;
	va_start( list, msg );
	vsprintf_s( buff, sizeof( buff ), msg, list );
	va_end( list );

	color_print( *( DWORD** )( ( uintptr_t )xti::g_console + 8 ), buff, color );
};

// this is so fucking ugly, but this is the only solution i could find.
// Basically, the animtime stops if the entity is dead or has frozen,
// GetClientTime ticks at the same speed as animtime, but considering that clienttime is always ticking, we
// can use this to make a dormancy check.
bool xtu::dormant( sdk::cl_entity_s* entity, sdk::cl_entity_s* local ) {
	return ( entity->curstate.messagenum < local->curstate.messagenum );
};

bool xtu::stopped_animation( sdk::cl_entity_s* entity ) {
	return ( xti::g_engine->GetClientTime( ) > entity->curstate.animtime + 0.18f );
};

bool xtu::is_player_dead( ) { return ( xti::g_playermove->dead || xti::g_playermove->iuser1 != 0 ); };
bool xtu::is_player_movetype( ) { return ( xti::g_playermove->movetype & MOVETYPE_NOCLIP || ( xti::g_playermove->movetype == MOVETYPE_FLY ) ); };
bool xtu::is_player_inwater( ) { return ( xti::g_playermove->flags & FL_INWATER ); }
bool xtu::is_player_dead_non_observer( )
{
	return xti::g_playermove->dead;
};
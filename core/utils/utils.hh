#include <psapi.h>
#include <string_view>
#include <sdk/c_vector.hh>
#include <sdk/c_usercmd.hh>
#include <sdk/c_engine_funcs.hh>
#include "address.hh"

namespace xtu {

	extern float viewangles_x;
	extern float viewangles_y;

	std::uintptr_t find_pattern( const std::string_view& from, const std::string_view& pattern ) noexcept;

	std::uintptr_t find_string( const std::string_view& from, const std::string_view& string ) noexcept;

	address find( const std::string_view& from, const std::string_view& pattern ) noexcept;

	std::uintptr_t get_module( const char* sz_module );

	std::pair<std::uintptr_t, std::uintptr_t > get_module_info( const std::string_view& from );

	std::uintptr_t get_module_addr( const char* sz_module, const std::uintptr_t addr );

	std::uintptr_t get_module_base_address( const char* sz_module, void* ptr );

	void fix_movement( sdk::c_vector& base_angles, sdk::c_user_cmd* cmd );

	void overlay_unhook_function( void* func, bool rehook );

	double deg2rad( double deg );

	double rad2deg( double angle );

	float deg2radf( float deg );

	float rad2degf( float angle );

	void vector_angles( const sdk::c_vector& forward, sdk::c_vector& angles );

	void angle_vectors( const sdk::c_vector& angles, sdk::c_vector* forward );

	bool world_to_screen( sdk::c_vector in, sdk::c_vector& out );

	extern bool send_packet;

	void begin_read( void* buffer, int size );

	char* read_string( );

	double read_coord( );

	unsigned short read_short( );

	void usermsg( const char* message, void* func );

	void* usermsg_org( const char* message );

	void buffered_localise_text_string( const char* msg );

	int insert_color_change( DWORD* ecx, int a2 );

	int insert_text( void* ecx, const char* msg );

	int color_print( DWORD* thisptr, const char* msg, sdk::c_color32 color );

	void console_color_printf( sdk::c_color32 color, const char* msg, ... );

	BYTE read_byte( );

	char* util_get_map_name( );

	// this is so fucking ugly, but this is the only solution i could find.
// Basically, the animtime stops if the entity is dead or has frozen,
// GetClientTime ticks at the same speed as animtime, but considering that clienttime is always ticking, we
// can use this to make a dormancy check.
	bool dormant( sdk::cl_entity_s* entity, sdk::cl_entity_s* local );

	bool stopped_animation( sdk::cl_entity_s* entity );

	bool is_player_dead( );
	bool is_player_movetype( );
	bool is_player_inwater( );
	bool is_player_dead_non_observer( );
};
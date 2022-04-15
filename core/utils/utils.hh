#include <psapi.h>
#include <string_view>
#include <sdk/c_vector.hh>
#include <sdk/c_usercmd.hh>
#include <sdk/c_engine_funcs.hh>
#include "address.hh"

namespace xtu {
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

	bool world_to_screen( sdk::c_vector in, sdk::c_vector& out );

	extern bool send_packet;

	void begin_read( void* buffer, int size );

	char* read_string( );

	double read_coord( );

	void usermsg( const char* message, void* func );

	void* usermsg_org( const char* message );

	void buffered_localise_text_string( const char* msg );

	BYTE read_byte( );
};
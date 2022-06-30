#pragma once

namespace sdk {
	class c_steam_friends;
	class c_steam_utils;
	class c_steam_user;
	class c_steam_client;

	class c_steam_friends {
	public:
		virtual const char* get_persona_name( ) = 0;
	};

	class c_steam_user {
	public:
		virtual std::int32_t get_steam_user( ) = 0;
		virtual bool logged_on( ) = 0;
		virtual std::uint64_t get_steam_id( ) = 0;
	};

	class c_steam_client {
	public:
		virtual std::int32_t create_steam_pipe( ) = 0;
		virtual bool b_release_steam_pipe( std::int32_t pipe ) = 0;

		c_steam_friends* get_steam_friends( std::int32_t steam_user, std::int32_t steam_pipe, const char* version ) {
			return vtable< c_steam_friends* ( __thiscall* )( void*, std::int32_t, std::int32_t, const char* ) >( this, 8 )( this, steam_user, steam_pipe, version );
		};
	};
};
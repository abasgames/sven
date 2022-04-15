#pragma once

namespace sdk {
	class c_server_browser {
	public:
		virtual bool active( ) = 0;
		virtual bool join_game( int server_ip, int server_port ) = 0;
		virtual bool join_game( void* game_list, unsigned short server_index ) = 0;
		virtual bool open_game_info_dialog( void* gameList, unsigned int serverIndex ) = 0;
		virtual void close_game_info_dialog( void* gameList, unsigned int serverIndex ) = 0;
		virtual void close_all_game_info_dialogs( void ) = 0;
		virtual void create_dialog( void ) = 0;
		virtual void open( void ) = 0;
	};
};
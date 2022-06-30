#pragma once
#include <cstdint>
#include <sdk/sdk.hh>

namespace xth {
	extern int _weapon_id;
	extern bool stuck_dead_afk;
	extern bool stuck_air_afk;
	using fn_swap_buffers = bool( __stdcall* )( HDC hdc );
	bool __stdcall hk_swap_buffers( HDC hdc );

	using fn_create_move = void( __cdecl* )( float frametime, sdk::c_user_cmd* m_cmd, int active );
	void hk_create_move( float, sdk::c_user_cmd*, int );

	using fn_netchan_transmit = void( __cdecl* )( sdk::netchan_t* netchan, int length, byte* data );
	void hk_netchan_transmit( sdk::netchan_t*, int, byte* );

	using fn_netchan_can_packet = int( * )( sdk::netchan_t* chan );
	int hk_netchan_can_packet( sdk::netchan_t* chan );

	using fn_netchan_process = bool( * )( sdk::netchan_t* chan );
	bool hk_netchan_process( sdk::netchan_t* chan );

	using fn_votepopup_msgfunc = int( __thiscall* )( sdk::c_vote_popup* vote, const char* panel_name, int size, void* data );
	int __fastcall hk_votepopup_msgfunc( sdk::c_vote_popup* vote, void* edx, const char* panel_name, int size, void* data );

	using fn_votepopup_reset = void* ( __thiscall* )( void* ecx );
	void* __fastcall hk_votepopup_reset( void* ecx, void* edx );

	using fn_votepopup_update = void( __thiscall* )( void* ecx );
	void __fastcall hk_votepopup_update( void* ecx, void* edx );

	using fn_end_vote = int( * )( const char* message, int size, void* buffer );
	int hk_end_vote( const char* sz_message, int size, void* buffer );

	using fn_start_vote = int( * )( const char* message, int size, void* buffer );
	int hk_start_vote( const char* sz_message, int size, void* buffer );

	using fn_hud_post_run_cmd = void( * )( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed );
	void hk_hud_post_run_cmd( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed );

	using fn_calc_ref_def = void( * )( sdk::ref_params_s* pparams );
	void hk_calc_ref_def( sdk::ref_params_s* pparams );

	using fn_hud_redraw = void( * )( float time, int intermission );
	void hk_hud_redraw( float time, int intermission );

	using fn_scr_updatescreen = void( * )( );
	void hk_scr_updatescreen( );

	using fn_chat_color = void* ( * )( int );
	void* hk_chat_color( int );

	using fn_server_votemanager_startvote = void( __thiscall* )( void* ecx, void* str, int unk );
	void __fastcall hk_server_votemanager_playervoted( void* ecx, void* edx, void* str, int unk );

	using fn_server_votemanager_endvote = int( __thiscall* )( void* ecx );
	int __fastcall hk_server_votemanager_endvote( void* ecx, void* edx );

	using fn_set_fov = int( * )( const char* sz_message, int size, void* buffer );
	int hk_set_fov( const char* sz_message, int size, void* buffer );

	using fn_say_text = int( * )( const char* sz_message, int size, void* buffer );
	int hk_say_text( const char* sz_message, int size, void* buffer );

	void setup_hooks( );
	void disable_hooks( );

	void create( );
	void enable( );
	void disable( );
	void disable( void* hookptr );
	void enable( void* hookptr );
	void add( void*, void*, void** );

	namespace swapbuffers {
		void hook( );
		void unhook( );
		void glhooks( );
	}

	namespace hud {
		void hook( );
		void unhook( );
		void hudhooks( );
	}

	// unhook isnt needed for the netchan since it uses the minhook.
	namespace netchan {
		void hook( );
	}

	namespace cl {
		void hook( );
		void unhook( );
	}

	namespace votemenu {
		void hook( );

		void rehook( );
		void unhook( );
		void serverhook( );
	}

	namespace scr {
		void hook( );
	}

	namespace color {
		void hook( );
	};

	extern std::uintptr_t address_swap_buffers;
};
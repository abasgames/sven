#pragma once
#include <cstdint>
#include <sdk/sdk.hh>

namespace xth {
	using fn_swap_buffers = bool( __stdcall* )( HDC hdc );
	bool __stdcall hk_swap_buffers( HDC hdc );

	using fn_create_move = void( __cdecl* )( float frametime, sdk::c_user_cmd* m_cmd, int active );
	void hk_create_move( float, sdk::c_user_cmd*, int );

	using fn_net_chan_transmit = void( __cdecl* )( sdk::netchan_t* netchan, int length, byte* data );
	void hk_netchan_transmit( sdk::netchan_t*, int, byte* );

	using fn_net_can_packet = int( * )( sdk::netchan_t* chan );
	int hk_net_can_packet( sdk::netchan_t* chan );

	using fn_votepopup_msgfunc = int( __thiscall* )( sdk::c_vote_popup* vote, const char* panel_name, int size, void* data );
	int __fastcall hk_votepopup_msgfunc( sdk::c_vote_popup* vote, void* edx, const char* panel_name, int size, void* data );

	using fn_hud_post_run_cmd = void( * )( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed );
	void hk_hud_post_run_cmd( sdk::local_state_t* from, sdk::local_state_t* to, sdk::usercmd_s* cmd, int runfuncs, double time, unsigned int random_seed );

	using fn_calc_ref_def = void( * )( sdk::ref_params_s* pparams );
	void hk_calc_ref_def( sdk::ref_params_s* pparams );

	using fn_studio_setup_bones = void( __stdcall* )( );
	void __stdcall hk_studio_setup_bones( );

	using fn_scr_updatescreen = void( * )( );
	void hk_scr_updatescreen( );

	void setup_hooks( );
	void disable_hooks( );

	void create( );
	void enable( );
	void disable( );
	void add( void*, void*, void** );

	namespace swapbuffers {
		void hook( );
		void unhook( );
	}

	namespace hud {
		void hook( );
		void unhook( );
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
	}

	namespace scr {
		void hook( );
	}

	extern std::uintptr_t address_swap_buffers;
};
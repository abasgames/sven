#pragma once

namespace options {

	// { "backwards", "sideways", "back jitter", "slowspin", "fastspin", "lisp", "fake left", "fake right" };
	enum antiaim_yaw_modes : int {
		y_none,
		y_zero,
		y_backwards,
		y_sideways,
		y_back_jitter,
		y_slowspin,
		y_fastspin,
		y_goldenspin,
		y_random,
		y_fakeleft,
		y_fakeright,
		y_switch,
		y_lisp
	};

	// { "emotion", "up", "down", "fakedown", "270.0f" };
	enum antiaim_pitch_modes : int {
		x_none,
		x_zero,
		x_emotion,
		x_up,
		x_down,
		x_up2,
		x_fakedown,
		x_fakeup,
		x_varie,
		x_lisp
	};

	enum antiafk_modes : int {
		afk_sidemove,
		afk_forwardmove,
		afk_both,
		afk_roaming,
		afk_kill,
		afk_gibme
	};

	enum airstuck_modes : int {
		airstuck_freeze,
		airstuck_packet,
		airstuck_time,
		airstuck_complete_freeze,
		airstuck_process
	};
	extern bool show_menu;
	extern int show_menu_key;
	extern int show_menu_tab;
	extern bool player_important[ 64 ];
	extern float player_important_hue[ 64 ];
};
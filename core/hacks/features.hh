#pragma once

namespace options {

	// { "backwards", "sideways", "back jitter", "slowspin", "fastspin", "lisp", "fake left", "fake right" };
	enum antiaim_yaw_modes : int {
		y_backwards,
		y_sideways,
		y_back_jitter,
		y_slowspin,
		y_fastspin,
		y_random,
		y_fakeleft,
		y_fakeright,
		y_switch
	};

	// { "emotion", "up", "down", "fakedown", "270.0f" };
	enum antiaim_pitch_modes : int {
		x_emotion,
		x_up,
		x_down,
		x_fakedown,
		x_varie,
		x_lisp
	};

	enum antiafk_modes : int {
		afk_sidemove,
		afk_forwardmove,
		afk_both,
		afk_roaming
	};
	extern bool show_menu;
	extern int show_menu_key;
	extern int show_menu_tab;

	namespace general {
		extern bool airstuck;
		extern int airstuck_key;

		extern bool autogauss;
		extern int autogauss_key;

		extern bool antiafk;
		extern int antiafk_mode;

		extern bool slowwalk;
		extern int slowwalk_key;

		extern bool bhop;

		extern bool autostrafe;
		extern int autostrafe_key;
		extern int autostrafe_mode;

		extern bool last_inventory;
		extern int last_inventory_key;

		extern bool speedhack;
		extern int speedhack_key;
		extern int speedhack_mode;

		extern bool jumpbug;
	};

	namespace antiaim {
		extern bool enabled;

		extern int antiaim_yaw;
		extern int antiaim_pitch;

		extern bool clamp;
	};

	namespace fakelag {
		extern bool enabled;

		extern int fakelag_mode;
		extern int fakelag_key;
	};

	// == // == // == //

	namespace visuals {
		extern bool enabled;
		extern bool names;
		extern bool healthbar;
		extern bool weapons;

		extern bool box;
		extern int box_mode;

		extern bool skeleton;
		extern bool chams;
		extern bool xqz;
		extern bool glow;
		extern bool dlight;
		extern bool debug;
		extern bool brightness;
	};

	namespace mobs {
		extern bool enabled;
		extern bool names;
		extern bool box;
		extern int box_mode;
		extern bool skeleton;
		extern bool chams;
		extern bool xqz;
		extern bool glow;
		extern bool dlight;
	};

	namespace recorder {
		extern bool enabled;

		extern int record_record;
		extern int record_play;

		extern bool record_clear;
	};
};
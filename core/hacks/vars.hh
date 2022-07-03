#include "config.hh"

struct vars_t {
	ADD_VAR( menu, bool, false );
	ADD_VAR( menu_key, int, static_cast< int >( 0x2d ) );
	ADD_VAR( menu_index, int, 0 );

	ADD_VAR( airstuck, bool, false );
	ADD_VAR( airstuck_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( airstuck_mode, int, 3 );
	ADD_VAR( airstuck_air_afk, bool, false );
	ADD_VAR( airstuck_dead_afk, bool, false );
	ADD_VAR( airstuck_on_death, bool, false );
	ADD_VAR( airstuck_on_death_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( autogauss, bool, false );
	ADD_VAR( autogauss_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( selfsink, bool, false );
	ADD_VAR( selfsink_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( antiafk, bool, false );
	ADD_VAR( antiafk_mode, int, 0 );
	ADD_VAR( antiafk_sidemove, float, 5.0f );
	ADD_VAR( antiafk_forwardmove, float, 5.0f );

	ADD_VAR( autostrafe, bool, true );
	ADD_VAR( autostrafe_key, int, static_cast< int >( 0x20 ) );
	ADD_VAR( autostrafe_mode, int, 0 );

	ADD_VAR( fastswitch, bool, false );
	ADD_VAR( fastswitch_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( speedhack, bool, false );
	ADD_VAR( speedhack_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( speedhack_fast_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( speedhack_slow_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( speedhack_mode, int, 0 );
	ADD_VAR( speedhack_factor, float, 10.0f );
	ADD_VAR( speedhack_multiplier, float, 50.0f );

	ADD_VAR( jumpbug, bool, true );
	ADD_VAR( bunnyhop, bool, true );

	ADD_VAR( autocross, bool, false );
	ADD_VAR( autocross_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( corpse, bool, false );
	ADD_VAR( corpse_mode, int, 0 );
	ADD_VAR( corpse_angle_x, float, 0.0f );
	ADD_VAR( corpse_angle_z, float, 0.0f );
	ADD_VAR( corpse_spin_barrel, float, 0.0f );
	ADD_VAR( corpse_spin, float, 20.0f );

	ADD_VAR( slowwalk, bool, false );
	ADD_VAR( slowwalk_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( slowwalk_speed, float, 30.0f );

	ADD_VAR( usespam, bool, false );
	ADD_VAR( usespam_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( flashspam, bool, false );
	ADD_VAR( flashspam_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( follow_player, bool, false );
	ADD_VAR( follow_important, bool, false );
	ADD_VAR( follow_order, bool, false );
	ADD_VAR( follow_steal_skin, bool, false );
	ADD_VAR( follow_distance, float, 30.0f );

	ADD_VAR( chatspam, bool, false );
	ADD_VAR( airrun, bool, false );
	ADD_VAR( fastrun, bool, false );
	ADD_VAR( scr, bool, false );
	ADD_VAR( notifications, bool, true );

	ADD_VAR( crosshair, bool, true );
	ADD_VAR( crosshair_hue, float, 0.0f );
	ADD_VAR( crosshair_sat, float, 1.0f );
	ADD_VAR( crosshair_gap, float, 5.0f );

	ADD_VAR( rev_expl, bool, false );
	ADD_VAR( rev_expl_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( rev_expl_factor, int, 45 );
	ADD_VAR( rev_expl_factor2, float, 1000.0f );

	ADD_VAR( dupeglitch, bool, false );
	ADD_VAR( dupeglitch_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( antiaim, bool, false );
	ADD_VAR( antiaim_yaw, int, 0 );
	ADD_VAR( antiaim_pitch, int, 0 );
	ADD_VAR( antiaim_clamp, bool, true );
	ADD_VAR( antiaim_clamp_tp, bool, true );
	ADD_VAR( antiaim_simulate, bool, false );

	ADD_VAR( fakelag, bool, false );
	ADD_VAR( fakelag_mode, int, 0 );
	ADD_VAR( fakelag_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( fakelag_factor, int, 15 );
	ADD_VAR( fakelag_toggle, bool, false );

	ADD_VAR( mirror, bool, false );
	ADD_VAR( mirror_key, int, static_cast< int >( 0x1b ) );

	ADD_VAR( autohomie, bool, false );

	ADD_VAR( visuals, bool, true );
	ADD_VAR( visuals_hue, float, 0.45f );
	ADD_VAR( visuals_name, bool, true );
	ADD_VAR( visuals_box, bool, true );
	ADD_VAR( visuals_health, bool, true );
	ADD_VAR( visuals_self, bool, false );
	ADD_VAR( visuals_dlight, bool, false );
	ADD_VAR( visuals_dlight_hue, float, 0.45f );
	ADD_VAR( visuals_dlight_height, float, 15.0f );
	ADD_VAR( visuals_angles, bool, false );
	ADD_VAR( visuals_origin, bool, false );
	ADD_VAR( visuals_minsmaxs, bool, false );
	ADD_VAR( visuals_snaplines, bool, false );
	ADD_VAR( visuals_debug, bool, false );
	ADD_VAR( visuals_skininfo, bool, false );
	ADD_VAR( visuals_dormancy, bool, false );
	ADD_VAR( visuals_corpse, bool, false );
	ADD_VAR( visuals_only_important, bool, false );

	ADD_VAR( world_brightness, bool, false );
	ADD_VAR( world_brightness_factor, float, 1.0f );
	ADD_VAR( world, bool, true );
	ADD_VAR( world_ammo, bool, true );
	ADD_VAR( world_weapons, bool, false );
	ADD_VAR( world_misc, bool, true );

	ADD_VAR( mobs, bool, true );
	ADD_VAR( mobs_name, bool, true );
	ADD_VAR( mobs_box, bool, true );
	ADD_VAR( mobs_dlight, bool, false );
	ADD_VAR( mobs_dlight_hue, float, 0.45f );
	ADD_VAR( mobs_dlight_height, float, 15.0f );

	ADD_VAR( movement, bool, false );
	ADD_VAR( movement_record, int, static_cast< int >( 0x1b ) );
	ADD_VAR( movement_play, int, static_cast< int >( 0x1b ) );
	ADD_VAR( movement_clear, bool, false );

	ADD_VAR( vote, bool, false );
	ADD_VAR( vote_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( vote_auto, bool, false );
	ADD_VAR( vote_disable, bool, false );
	ADD_VAR( vote_mode, int, 0 );
	ADD_VAR( vote_kill_key, int, static_cast< int >( 0x1b ) );
	ADD_VAR( vote_kill, bool, false );

	ADD_VAR( model_color_top, float, 0.0f );
	ADD_VAR( model_color_bottom, float, 0.0f );

	ADD_VAR( menu_pos_x, float, 10.0f );
	ADD_VAR( menu_pos_y, float, 10.0f );
};

inline vars_t vars;
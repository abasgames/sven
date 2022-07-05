#pragma once

namespace sdk {
	typedef struct {
		float time;
		float frame_time;
		float force_retouch;
		unsigned int map_name; // no clue why this is an unsigned int, and not an unsigned char
		unsigned int start_spot;
		float death_match;
		float coop;
		float team_play;
		float server_flags;
		float found_secrets;
		sdk::c_vector vec_forward;
		sdk::c_vector vec_up;
		sdk::c_vector vec_right;
		float trace_allsolid;
		float trace_startsolid;
		float trace_fraction;
		sdk::c_vector trace_endpos;
		sdk::c_vector trace_plane_normal;
		float trace_plane_dist;
		void* trace_ent;
		float trace_inopen;
		float trace_inwater;
		int	trace_hitgroup;
		int	trace_flags;
		int	change_level;	// transition in progress when true (was msg_entity)
		int	cd_audio_track;
		int	max_clients;
		int	max_entities;
		const char* string_base;
		void* save_data;	// (SAVERESTOREDATA *) pointer
		sdk::c_vector vec_landmark_offset;
	}globalvars_t;

	// it was a pain finding the globalvars pointer, but it's done.
	// string search for "ammo_357" in "client.dll" and you'll find it.
	/*
	.text:100130A0 8B 41 04					mov     eax, [ecx+4]
	.text:100130A3 8B 10                    mov     edx, [eax]
	.text:100130A5 A1 1C 12 5C 10           mov     eax, dword_105C121C
	*/
}
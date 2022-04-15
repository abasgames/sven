#pragma once

namespace sdk {
	typedef struct {
		float		time;
		float		frametime;
		float		force_retouch;
		unsigned int mapname;
		unsigned int		startspot;
		float		deathmatch;
		float		coop;
		float		teamplay;
		float		serverflags;
		float		found_secrets;
		sdk::c_vector v_forward;
		sdk::c_vector		v_up;
		sdk::c_vector		v_right;
		float		trace_allsolid;
		float		trace_startsolid;
		float		trace_fraction;
		sdk::c_vector		trace_endpos;
		sdk::c_vector		trace_plane_normal;
		float		trace_plane_dist;
		void* trace_ent;
		float		trace_inopen;
		float		trace_inwater;
		int		trace_hitgroup;
		int		trace_flags;
		int		changelevel;	// transition in progress when true (was msg_entity)
		int		cdAudioTrack;
		int		maxClients;
		int		maxEntities;
		const char* pStringBase;
		void* pSaveData;	// (SAVERESTOREDATA *) pointer
		sdk::c_vector vecLandmarkOffset;
	}globalvars_t;
}
#pragma once
#define	MAX_PHYSENTS 600 		  // Must have room for all entities in the world.
#define MAX_MOVEENTS 64
#define	MAX_CLIP_PLANES	5

#define PM_NORMAL			0x00000000
#define PM_STUDIO_IGNORE	0x00000001		// Skip studio models
#define PM_STUDIO_BOX		0x00000002		// Use boxes for non-complex studio models (even in traceline)
#define PM_GLASS_IGNORE		0x00000004		// Ignore entities with non-normal rendermode
#define PM_WORLD_ONLY		0x00000008		// Only trace against the world

// Values for flags parameter of PM_TraceLine
#define PM_TRACELINE_ANYVISIBLE		0
#define PM_TRACELINE_PHYSENTSONLY	1
namespace sdk {
	typedef struct {
		sdk::c_vector normal;
		float dist;
	}pmplane_t;

	typedef struct pmtrace_s {
		int allsolid;	      // if true, plane is not valid
		int startsolid;	      // if true, the initial point was in a solid area
		int inopen, inwater;  // End point is in empty space or in water
		float fraction;		  // time completed, 1.0 = didn't hit anything
		sdk::c_vector endpos;			  // final position
		pmplane_t plane;		      // surface normal at impact
		int ent;			  // entity at impact
		sdk::c_vector deltavelocity;    // Change in player's velocity caused by impact.  
		int hitgroup;
	}pmtrace_t;

	typedef struct physent_s
	{
		char name[ 32 ];             // Name of model, or "player" or "world".
		int player;
		sdk::c_vector origin;               // Model's origin in world coordinates.
		struct model_s* model;		          // only for bsp models
		struct model_s* studiomodel;         // SOLID_BBOX, but studio clip intersections.
		sdk::c_vector mins, maxs;	          // only for non-bsp models
		int	 info;		          // For client or server to use to identify (index into edicts or cl_entities)
		sdk::c_vector angles;               // rotated entities need this info for hull testing to work.

		int solid;				  // Triggers and func_door type WATER brushes are SOLID_NOT
		int skin;                 // BSP Contents for such things like fun_door water brushes.
		int rendermode;			  // So we can ignore glass
		float frame;
		int	sequence;
		byte controller[ 4 ];
		byte blending[ 2 ];

		int movetype;
		int takedamage;
		int blooddecal;
		int team;
		int classnumber;

		// For mods
		int iuser1;
		int iuser2;
		int iuser3;
		int iuser4;
		float fuser1;
		float fuser2;
		float fuser3;
		float fuser4;
		sdk::c_vector vuser1;
		sdk::c_vector vuser2;
		sdk::c_vector vuser3;
		sdk::c_vector vuser4;
	} physent_t;

	struct player_move_s {
		int player_index; // So we don't try to run the PM_CheckStuck nudging too quickly.
		int server;  // For debugging, are we running physics code on server side?

		int multiplayer; // 1 == multiplayer server
		float time;			  // realtime on host, for reckoning duck timing
		float frametime;	  // Duration of this frame

		sdk::c_vector forward, right, up; // Vectors for angles
		// player state
		sdk::c_vector origin;		 // Movement origin.
		sdk::c_vector angles;		 // Movement view angles.
		sdk::c_vector oldangles;	 // Angles before movement view angles were looked at.
		sdk::c_vector velocity;	 // Current movement direction.
		sdk::c_vector movedir;		 // For waterjumping, a forced forward velocity so we can fly over lip of ledge.
		sdk::c_vector basevelocity; // Velocity of the conveyor we are standing, e.g.

		// For ducking/dead
		sdk::c_vector view_ofs;  // Our eye position.
		float flDuckTime; // Time we started duck
		int bInDuck; // In process of ducking or ducked already?

		// For walking/falling
		int flTimeStepSound; // Next time we can play a step sound
		int iStepLeft;

		float flFallVelocity;
		sdk::c_vector punchangle;

		float flSwimTime;

		float flNextPrimaryAttack;

		int effects; // MUZZLE FLASH, e.g.

		int flags;	   // FL_ONGROUND, FL_DUCKING, etc.
		int usehull;   // 0 = regular player hull, 1 = ducked player hull, 2 = point hull
		float gravity; // Our current gravity and friction.
		float friction;
		int oldbuttons;		 // Buttons last usercmd
		float waterjumptime; // Amount of time left in jumping out of water cycle.
		int dead;		 // Are we a dead player?
		int deadflag;
		int spectator; // Should we use spectator physics model?
		int movetype;  // Our movement type, NOCLIP, WALK, FLY

		int onground;
		int waterlevel;
		int watertype;
		int oldwaterlevel;

		char sztexturename[256];
		char chtexturetype;

		float maxspeed;
		float clientmaxspeed; // Player specific maxspeed

		// For mods
		int iuser1;
		int iuser2;
		int iuser3;
		int iuser4;
		float fuser1;
		float fuser2;
		float fuser3;
		float fuser4;
		sdk::c_vector vuser1;
		sdk::c_vector vuser2;
		sdk::c_vector vuser3;
		sdk::c_vector vuser4;
	};

	using player_move_t = player_move_s;
}
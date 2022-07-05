#pragma once

#define MOVETYPE_NONE 0 // never moves
//#define	MOVETYPE_ANGLENOCLIP	1
//#define	MOVETYPE_ANGLECLIP		2
#define MOVETYPE_WALK 3			  // Player only - moving on the ground
#define MOVETYPE_STEP 4			  // gravity, special edge handling -- monsters use this
#define MOVETYPE_FLY 5			  // No gravity, but still collides with stuff
#define MOVETYPE_TOSS 6			  // gravity/collisions
#define MOVETYPE_PUSH 7			  // no clip to world, push and crush
#define MOVETYPE_NOCLIP 8		  // No gravity, no collisions, still do velocity/avelocity
#define MOVETYPE_FLYMISSILE 9	  // extra size to monsters
#define MOVETYPE_BOUNCE 10		  // Just like Toss, but reflect velocity when contacting surfaces
#define MOVETYPE_BOUNCEMISSILE 11 // bounce w/o gravity
#define MOVETYPE_FOLLOW 12		  // track movement of aiment
#define MOVETYPE_PUSHSTEP 13	  // BSP model that needs physics/world collisions (uses nearest hull for world collision)

#define FL_FLY (1 << 0)	 // Changes the SV_Movestep() behavior to not need to be on ground
#define FL_SWIM (1 << 1) // Changes the SV_Movestep() behavior to not need to be on ground (but stay in water)
#define FL_CONVEYOR (1 << 2)
#define FL_CLIENT (1 << 3)
#define FL_INWATER (1 << 4)
#define FL_MONSTER (1 << 5)
#define FL_GODMODE (1 << 6)
#define FL_NOTARGET (1 << 7)
#define FL_SKIPLOCALHOST (1 << 8)  // Don't send entity to local host, it's predicting this entity itself
#define FL_ONGROUND (1 << 9)	   // At rest / on the ground
#define FL_PARTIALGROUND (1 << 10) // not all corners are valid
#define FL_WATERJUMP (1 << 11)	   // player jumping out of water
#define FL_FROZEN (1 << 12)		   // Player is frozen for 3rd person camera
#define FL_FAKECLIENT (1 << 13)	   // JAC: fake client, simulated server side; don't send network messages to them
#define FL_DUCKING (1 << 14)	   // Player flag -- Player is fully crouched
#define FL_FLOAT (1 << 15)		   // Apply floating force to this entity when in water
#define FL_GRAPHED (1 << 16)	   // worldgraph has this ent listed as something that blocks a connection

// UNDONE: Do we need these?
#define FL_IMMUNE_WATER (1 << 17)
#define FL_IMMUNE_SLIME (1 << 18)
#define FL_IMMUNE_LAVA (1 << 19)

#define FL_PROXY (1 << 20)		  // This is a spectator proxy
#define FL_ALWAYSTHINK (1 << 21)  // Brush model flag -- call think every frame regardless of nextthink - ltime (for constantly changing velocity/path)
#define FL_BASEVELOCITY (1 << 22) // Base velocity has been applied this frame (used to convert base velocity into momentum)
#define FL_MONSTERCLIP (1 << 23)  // Only collide in with monsters who have FL_MONSTERCLIP set
#define FL_ONTRAIN (1 << 24)	  // Player is _controlling_ a train, so movement commands should be ignored on client during prediction.
#define FL_WORLDBRUSH (1 << 25)	  // Not moveable/removeable brush entity (really part of the world, but represented as an entity for transparency or something)
#define FL_SPECTATOR (1 << 26)	  // This client is a spectator, don't run touch functions, etc.
#define FL_CUSTOMENTITY (1 << 29) // This is a custom entity
#define FL_KILLME (1 << 30)		  // This entity is marked for death -- This allows the engine to kill ents at the appropriate time
#define FL_DORMANT (1 << 31)	  // Entity is dormant, no updates to client

namespace sdk {
	constexpr int in_attack = ( 1 << 0 );
	constexpr int in_jump = ( 1 << 1 );
	constexpr int in_duck = ( 1 << 2 );
	constexpr int in_forward = ( 1 << 3 );
	constexpr int in_back = ( 1 << 4 );
	constexpr int in_use = ( 1 << 5 );
	constexpr int in_cancel = ( 1 << 6 );
	constexpr int in_left = ( 1 << 7 );
	constexpr int in_right = ( 1 << 8 );
	constexpr int in_move_left = ( 1 << 9 );
	constexpr int in_move_right = ( 1 << 10 );
	constexpr int in_attack2 = ( 1 << 11 );
	constexpr int in_run = ( 1 << 12 );
	constexpr int in_reload = ( 1 << 13 );
	constexpr int in_alt1 = ( 1 << 14 );
	constexpr int in_alt2 = ( 1 << 15 );
	constexpr int in_score = ( 1 << 16 );
	constexpr int in_speed = ( 1 << 17 );
	constexpr int in_walk = ( 1 << 18 );
	constexpr int in_zoom = ( 1 << 19 );
	constexpr int in_weapon1 = ( 1 << 20 );
	constexpr int in_weapon2 = ( 1 << 21 );
	constexpr int in_bullrush = ( 1 << 22 ); // tar bort delay för duckning ifall man kör cmd->buttons |= bullrush;
	constexpr int in_grenade1 = ( 1 << 23 );
	constexpr int in_grenade2 = ( 1 << 24 );
	constexpr int in_attack3 = ( 1 << 25 );

	constexpr float state_standing = 28.0f;
	constexpr float state_ducking = 12.0f;

	typedef struct usercmd_s {
		short lerp_msec;
		byte msec;
		sdk::c_vector viewangles;
		float forwardmove;
		float sidemove;
		float upmove;
		byte lightlevel;
		unsigned short buttons;
		byte impulse;
		byte weaponselect;
		int impact_index;
		sdk::c_vector impact_position;
	}c_user_cmd;
};
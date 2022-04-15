#pragma once

namespace sdk {
	// Rendering constants
	enum
	{
		kRenderNormal,		 // src
		kRenderTransColor,	 // c*a+dest*(1-a)
		kRenderTransTexture, // src*a+dest*(1-a)
		kRenderGlow,		 // src*a+dest -- No Z buffer checks
		kRenderTransAlpha,	 // src*srca+dest*(1-srca)
		kRenderTransAdd,	 // src*a+dest
	};

	enum
	{
		kRenderFxNone = 0,
		kRenderFxPulseSlow,
		kRenderFxPulseFast,
		kRenderFxPulseSlowWide,
		kRenderFxPulseFastWide,
		kRenderFxFadeSlow,
		kRenderFxFadeFast,
		kRenderFxSolidSlow,
		kRenderFxSolidFast,
		kRenderFxStrobeSlow,
		kRenderFxStrobeFast,
		kRenderFxStrobeFaster,
		kRenderFxFlickerSlow,
		kRenderFxFlickerFast,
		kRenderFxNoDissipation,
		kRenderFxDistort,		  // Distort/scale/translate flicker
		kRenderFxHologram,		  // kRenderFxDistort + distance fade
		kRenderFxDeadPlayer,	  // kRenderAmt is the player index
		kRenderFxExplode,		  // Scale up really big!
		kRenderFxGlowShell,		  // Glowing Shell
		kRenderFxClampMinScale,	  // Keep this sprite from getting very small (SPRITES only!)
		kRenderFxLightMultiplier, //CTM !!!CZERO added to tell the studiorender that the value in iuser2 is a lightmultiplier
	};

	typedef struct efrag_s
	{
		struct mleaf_s* leaf;
		struct efrag_s* leafnext;
		struct cl_entity_s* entity;
		struct efrag_s* entnext;
	} efrag_t;

	typedef struct
	{
		byte					mouthopen;		// 0 = mouth closed, 255 = mouth agape
		byte					sndcount;		// counter for running average
		int						sndavg;			// running average
	} mouth_t;

	typedef struct
	{
		float					prevanimtime;
		float					sequencetime;
		byte					prevseqblending[ 2 ];
		sdk::c_vector prevorigin;
		sdk::c_vector					prevangles;

		int						prevsequence;
		float					prevframe;

		byte					prevcontroller[ 4 ];
		byte					prevblending[ 2 ];
	} latchedvars_t;

	typedef struct
	{
		// Time stamp for this movement
		float					animtime;
		sdk::c_vector					origin;
		sdk::c_vector					angles;
	} position_history_t;

	typedef struct
	{
		byte r, g, b;
	} color24;

	typedef struct
	{
		unsigned r, g, b, a;
	} colorVec;

	typedef struct cl_entity_s cl_entity_t;

	typedef struct entity_state_s entity_state_t;

	struct entity_state_s
	{
		int			entityType;
		int			number;
		float		msg_time;
		int			messagenum;
		sdk::c_vector origin;
		sdk::c_vector angles;
		int			modelindex;
		int			sequence;
		float		frame;
		int			colormap;
		short		skin;
		short		solid;
		int			effects;
		float		scale;
		byte		eflags;
		int			rendermode;
		int			renderamt;
		color24		rendercolor;
		int			renderfx;
		int			movetype;
		float		animtime;
		float		framerate;
		int			body;
		byte		controller[ 4 ];
		byte		blending[ 4 ];
		sdk::c_vector		velocity;
		sdk::c_vector		mins;
		sdk::c_vector		maxs;
		int			aiment;
		int			owner;
		float		friction;
		float		gravity;
		int			team;
		int			playerclass;
		int			health;
		bool		spectator;
		int         weaponmodel;
		int			gaitsequence;
		sdk::c_vector		basevelocity;
		int			usehull;
		int			oldbuttons;
		int			onground;
		int			iStepLeft;
		float		flFallVelocity;
		float		fov;
		int			weaponanim;
		sdk::c_vector				startpos;
		sdk::c_vector				endpos;
		float				impacttime;
		float				starttime;
		int			iuser1;
		int			iuser2;
		int			iuser3;
		int			iuser4;
		float		fuser1;
		float		fuser2;
		float		fuser3;
		float		fuser4;
		sdk::c_vector		vuser1;
		sdk::c_vector		vuser2;
		sdk::c_vector		vuser3;
		sdk::c_vector		vuser4;
	};

	typedef struct clientdata_s
	{
		sdk::c_vector		origin;
		sdk::c_vector		velocity;
		int					viewmodel;
		sdk::c_vector		punchangle;
		int					flags;
		int					waterlevel;
		int					watertype;
		sdk::c_vector		view_ofs;
		float				health;
		int					bInDuck;
		int					weapons;
		int					flTimeStepSound;
		int					flDuckTime;
		int					flSwimTime;
		int					waterjumptime;
		float				maxspeed;
		float				fov;
		int					weaponanim;
		int					m_iId;
		int					ammo_shells;
		int					ammo_nails;
		int					ammo_cells;
		int					ammo_rockets;
		float				m_flNextAttack;
		int					tfstate;
		int					pushmsec;
		int					deadflag;
		char				physinfo[ 256 ];
		int					iuser1;
		int					iuser2;
		int					iuser3;
		int					iuser4;
		float				fuser1;
		float				fuser2;
		float				fuser3;
		float				fuser4;
		sdk::c_vector		vuser1;
		sdk::c_vector		vuser2;
		sdk::c_vector		vuser3;
		sdk::c_vector		vuser4;
	} clientdata_t;

	struct cl_entity_s {
		int						index; 
		bool				player; 
		entity_state_t			baseline;  
		entity_state_t			prevstate; 
		entity_state_t			curstate;  
		int						current_position; 
		position_history_t		ph[ 64 ]; 
		mouth_t					mouth;		
		latchedvars_t			latched;	
		float					lastmove;
		sdk::c_vector					origin;
		sdk::c_vector					angles;
		sdk::c_vector					attachment[ 4 ];
		int						trivial_accept;
		struct model_s* model;			
		struct efrag_s* efrag;			
		struct mnode_s* topnode;		
		float					syncbase;		
		int						visframe;		
		colorVec				cvFloorColor;

		int entity_type_index( ) {
			return *( int* )( ( uintptr_t )this + 0x60 );
		}
	};

	typedef struct weapon_data_s
	{
		int			m_iId;
		int			m_iClip;
		float		m_flNextPrimaryAttack;
		float		m_flNextSecondaryAttack;
		float		m_flTimeWeaponIdle;
		int			m_fInReload;
		int			m_fInSpecialReload;
		float		m_flNextReload;
		float		m_flPumpTime;
		float		m_fReloadTime;
		float		m_fAimedDamage;
		float		m_fNextAimBonus;
		int			m_fInZoom;
		int			m_iWeaponState;
		int			iuser1;
		int			iuser2;
		int			iuser3;
		int			iuser4;
		float		fuser1;
		float		fuser2;
		float		fuser3;
		float		fuser4;
	} weapon_data_t;


	typedef struct local_state_s
	{
		entity_state_t playerstate;
		clientdata_t   client;
		weapon_data_t  weapondata[ 64 ];
	} local_state_t;
};
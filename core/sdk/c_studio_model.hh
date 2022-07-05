#pragma once
#define STUDIO_RENDER 1
#define STUDIO_EVENTS 2

// lighting options
#define STUDIO_NF_FLATSHADE		0x0001
#define STUDIO_NF_CHROME		0x0002
#define STUDIO_NF_FULLBRIGHT	0x0004
#define STUDIO_NF_NOMIPS        0x0008
#define STUDIO_NF_ALPHA         0x0010
#define STUDIO_NF_ADDITIVE      0x0020
#define STUDIO_NF_MASKED        0x0040
namespace sdk {
	typedef struct r_studio_interface_s
	{
		int				version;
		int				( *StudioDrawModel )			( int flags );
		int				( *StudioDrawPlayer )			( int flags, struct entity_state_s* pplayer );
	} r_studio_interface_t;
	typedef enum
	{
		ST_SYNC = 0,
		ST_RAND
	} synctype_t;

	typedef struct
	{
		float		mins[3], maxs[3];
		float		origin[3];
		int			headnode[4];
		int			visleafs;		// not including the solid leaf 0
		int			firstface, numfaces;
	} dmodel_t;

	struct extra_player_info_t {
		short frags;
		short deaths;
		short playerclass;
		short health; // UNUSED currently, spectator UI would like this
		bool dead;	  // UNUSED currently, spectator UI would like this
		short teamnumber;
		char teamname[ 16 ];

		char pad[ 0x3c ];
	};

	typedef enum
	{
		mod_brush,
		mod_sprite,
		mod_alias,
		mod_studio
	} modtype_t;

	typedef struct mplane_s
	{
		vec3_t	normal;			// surface normal
		float	dist;			// closest appoach to origin
		byte	type;			// for texture axis selection and fast side tests
		byte	signbits;		// signx + signy<<1 + signz<<1
		byte	pad[2];
	} mplane_t;

	typedef struct
	{
		vec3_t		position;
	} mvertex_t;

	typedef struct
	{
		unsigned short	v[2];
		unsigned int	cachededgeoffset;
	} medge_t;

	typedef struct texture_s
	{
		char		name[16];
		unsigned	width, height;
		int			anim_total;				// total tenths in sequence ( 0 = no)
		int			anim_min, anim_max;		// time for this frame min <=time< max
		struct texture_s* anim_next;		// in the animation sequence
		struct texture_s* alternate_anims;	// bmodels in frame 1 use these
		unsigned	offsets[4];		// four mip maps stored
		unsigned	paloffset;
	} texture_t;

	typedef struct
	{
		float		vecs[2][4];		// [s/t] unit vectors in world space. 
									// [i][3] is the s/t offset relative to the origin.
									// s or t = dot(3Dpoint,vecs[i])+vecs[i][3]
		float		mipadjust;		// ?? mipmap limits for very small surfaces
		texture_t* texture;
		int			flags;			// sky or slime, no lightmap or 256 subdivision
	} mtexinfo_t;

	typedef struct mnode_s
	{
		// common with leaf
		int			contents;		// 0, to differentiate from leafs
		int			visframe;		// node needs to be traversed if current

		short		minmaxs[6];		// for bounding box culling

		struct mnode_s* parent;

		// node specific
		mplane_t* plane;
		struct mnode_s* children[2];

		unsigned short		firstsurface;
		unsigned short		numsurfaces;
	} mnode_t;

	typedef struct hull_s
	{
		struct dclipnode_t* clipnodes;
		mplane_t* planes;
		int			firstclipnode;
		int			lastclipnode;
		vec3_t		clip_mins;
		vec3_t		clip_maxs;
	} hull_t;

	typedef struct cache_user_s
	{
		void* data;
	} cache_user_t;

	typedef struct model_s
	{
		char		name[64];
		int	needload;		// bmodels and sprites don't cache normally

		modtype_t	type;
		int			numframes;
		synctype_t	synctype;

		int			flags;

		//
		// volume occupied by the model
		//		
		vec3_t		mins, maxs;
		float		radius;

		//
		// brush model
		//
		int			firstmodelsurface, nummodelsurfaces;

		int			numsubmodels;
		dmodel_t* submodels;

		int			numplanes;
		mplane_t* planes;

		int			numleafs;		// number of visible leafs, not counting 0
		struct mleaf_s* leafs;

		int			numvertexes;
		mvertex_t* vertexes;

		int			numedges;
		medge_t* edges;

		int			numnodes;
		mnode_t* nodes;

		int			numtexinfo;
		mtexinfo_t* texinfo;

		int			numsurfaces;
		struct msurface_t* surfaces;

		int			numsurfedges;
		int* surfedges;

		int			numclipnodes;
		struct dclipnode_t* clipnodes;

		int			nummarksurfaces;
		msurface_t** marksurfaces;

		hull_t		hulls[4];

		int			numtextures;
		texture_t** textures;

		byte* visdata;

		color24* lightdata;

		char* entities;

		//
		// additional model data
		//
		cache_user_t	cache;		// only access through Mod_Extradata

	} model_t;

	typedef struct player_info_s
	{
		// User id on server
		int userid;

		// User info string
		char userinfo[256];

		// Name
		char name[32];

		// Spectator or not, unused
		int spectator;

		int ping;
		int packet_loss;

		// skin information
		char model[64];
		int topcolor;
		int bottomcolor;

		// last frame rendered
		int renderframe;

		// Gait frame estimation
		int gaitsequence;
		float gaitframe;
		float gaityaw;
		c_vector prevgaitorigin;

		customization_t customdata;

		char hashedcdkey[16];
		std::uint64_t m_nSteamID;
	} player_info_t;

	typedef struct
	{
		char				name[32];	// bone name for symbolic links
		int		 			parent;		// parent bone
		int					flags;		// ??
		int					bonecontroller[6];	// bone controller index, -1 == none
		float				value[6];	// default DoF values
		float				scale[6];   // scale for delta DoF values
	} mstudiobone_t;

	struct studiohdr_t
	{
		int					id;
		int					version;

		char				name[64];
		int					length;

		sdk::c_vector		eyeposition;	// ideal eye position
		sdk::c_vector		min;			// ideal movement hull size
		sdk::c_vector		max;

		sdk::c_vector		bbmin;			// clipping bounding box
		sdk::c_vector		bbmax;

		int					flags;

		int					numbones;			// bones
		int					boneindex;

		int					numbonecontrollers;		// bone controllers
		int					bonecontrollerindex;

		int					numhitboxes;			// complex bounding boxes
		int					hitboxindex;

		int					numseq;				// animation sequences
		int					seqindex;

		int					numseqgroups;		// demand loaded sequences
		int					seqgroupindex;

		int					numtextures;		// raw textures
		int					textureindex;
		int					texturedataindex;

		int					numskinref;			// replaceable textures
		int					numskinfamilies;
		int					skinindex;

		int					numbodyparts;
		int					bodypartindex;

		int					numattachments;		// queryable attachable points
		int					attachmentindex;

		int					soundtable;
		int					soundindex;
		int					soundgroups;
		int					soundgroupindex;

		int					numtransitions;		// animation node to animation node transition graph
		int					transitionindex;

		mstudiobone_t* get_bone( int i ) const { return ( mstudiobone_t* )( ( ( byte* )this ) + boneindex ) + i; };
	};

	typedef struct
	{
		char					name[ 64 ];
		int						flags;
		int						width;
		int						height;
		int						index;
	} mstudiotexture_t;

	typedef struct
	{
		char				name[32];
		int					type;
		int					bone;
		sdk::c_vector		org;	// attachment point
		sdk::c_vector		vectors[3];
	} mstudioattachment_t;

	typedef struct
	{
		char				name[64];

		int					type;

		float				boundingradius;

		int					nummesh;
		int					meshindex;

		int					numverts;		// number of unique vertices
		int					vertinfoindex;	// vertex bone info
		int					vertindex;		// vertex vec3_t
		int					numnorms;		// number of unique surface normals
		int					norminfoindex;	// normal bone info
		int					normindex;		// normal vec3_t

		int					numgroups;		// deformation groups
		int					groupindex;
	} mstudiomodel_t;

	typedef struct
	{
		char				name[64];
		int					nummodels;
		int					base;
		int					modelindex; // index into models array
	} mstudiobodyparts_t;

	typedef struct engine_studio_api_s
	{
		// Allocate number*size bytes and zero it
		void* ( *Mem_Calloc )				( int number, size_t size );
		// Check to see if pointer is in the cache
		void* ( *Cache_Check )				( struct cache_user_s* c );
		// Load file into cache ( can be swapped out on demand )
		void			( *LoadCacheFile )				( char* path, struct cache_user_s* cu );
		// Retrieve model pointer for the named model
		struct model_s* ( *Mod_ForName )				( const char* name, int crash_if_missing );
		// Retrieve pointer to studio model data block from a model
		studiohdr_t* ( *Mod_Extradata )				( struct model_s* mod );
		// Retrieve indexed model from client side model precache list
		struct model_s* ( *GetModelByIndex )			( int index );
		// Get entity that is set for rendering
		struct cl_entity_s* ( *GetCurrentEntity )		( void );
		// Get referenced player_info_t
		player_info_t* ( *PlayerInfo )			( int index );
		// Get most recently received player state data from network system
		struct entity_state_s* ( *GetPlayerState )		( int index );
		// Get viewentity
		struct cl_entity_s* ( *GetViewEntity )			( void );
		// Get current frame count, and last two timestampes on client
		void			( *GetTimes )					( int* framecount, double* current, double* old );
		// Get a pointer to a cvar by name
		struct cvar_s* ( *GetCvar )					( const char* name );
		// Get current render origin and view vectors ( up, right and vpn )
		void			( *GetViewInfo )				( float* origin, float* upv, float* rightv, float* vpnv );
		// Get sprite model used for applying chrome effect
		struct model_s* ( *GetChromeSprite )			( void );
		// Get model counters so we can incement instrumentation
		void			( *GetModelCounters )			( int** s, int** a );
		// Get software scaling coefficients
		void			( *GetAliasScale )				( float* x, float* y );

		// Get bone, light, alias, and rotation matrices
		float**** ( *StudioGetBoneTransform ) ( void );
		float**** ( *StudioGetLightTransform )( void );
		float*** ( *StudioGetAliasTransform ) ( void );
		float*** ( *StudioGetRotationMatrix ) ( void );

		// Set up body part, and get submodel pointers
		void			( *StudioSetupModel )			( int bodypart, void** ppbodypart, void** ppsubmodel );
		// Check if entity's bbox is in the view frustum
		int				( *StudioCheckBBox )			( void );
		// Apply lighting effects to model
		void			( *StudioDynamicLight )			( struct cl_entity_s* ent, struct alight_s* plight );
		void			( *StudioEntityLight )			( struct alight_s* plight );
		void			( *StudioSetupLighting )		( struct alight_s* plighting );

		// Draw mesh vertices
		void			( *StudioDrawPoints )			( void );

		// Draw hulls around bones
		void			( *StudioDrawHulls )			( void );
		// Draw bbox around studio models
		void			( *StudioDrawAbsBBox )			( void );
		// Draws bones
		void			( *StudioDrawBones )			( void );
		// Loads in appropriate texture for model
		void			( *StudioSetupSkin )			( void* ptexturehdr, int index );
		// Sets up for remapped colors
		void			( *StudioSetRemapColors )		( int top, int bottom );
		// Set's player model and returns model pointer
		struct model_s* ( *SetupPlayerModel )			( int index );
		// Fires any events embedded in animation
		void			( *StudioClientEvents )			( void );
		// Retrieve/set forced render effects flags
		int				( *GetForceFaceFlags )			( void );
		void			( *SetForceFaceFlags )			( int flags );
		// Tell engine the value of the studio model header
		void			( *StudioSetHeader )			( void* header );
		// Tell engine which model_t * is being renderered
		void			( *SetRenderModel )				( struct model_s* model );

		// Final state setup and restore for rendering
		void			( *SetupRenderer )				( int rendermode );
		void			( *RestoreRenderer )			( void );

		// Set render origin for applying chrome effect
		void			( *SetChromeOrigin )			( void );

		// True if using D3D/OpenGL
		int				( *IsHardware )					( void );

		// Only called by hardware interface
		void			( *GL_StudioDrawShadow )		( void );
		void			( *GL_SetRenderMode )			( int mode );

		void			( *StudioSetRenderamt )			( int iRenderamt ); 	//!!!CZERO added for rendering glass on viewmodels
		void			( *StudioSetCullState )			( int iCull );
		void			( *StudioRenderShadow )			( int iSprite, float* p1, float* p2, float* p3, float* p4 );
	} engine_studio_api_t;

	typedef struct server_studio_api_s
	{
		// Allocate number*size bytes and zero it
		void* ( *Mem_Calloc )				( int number, size_t size );
		// Check to see if pointer is in the cache
		void* ( *Cache_Check )				( struct cache_user_s* c );
		// Load file into cache ( can be swapped out on demand )
		void			( *LoadCacheFile )				( char* path, struct cache_user_s* cu );
		// Retrieve pointer to studio model data block from a model
		void* ( *Mod_Extradata )				( struct model_s* mod );
	} server_studio_api_t;


	extern r_studio_interface_t* pStudioAPI;

	// server blending
#define SV_BLENDING_INTERFACE_VERSION 1

	typedef struct sv_blending_interface_s
	{
		int				version;

		void			( *SV_StudioSetupBones )		( struct model_s* pModel,
			float				frame,
			int				sequence,
			const sdk::c_vector		angles,
			const	sdk::c_vector		origin,
			const	byte* pcontroller,
			const	byte* pblending,
			int				iBone,
			const void* pEdict );
	} sv_blending_interface_t;

	class c_studio_model_renderer {
	public:
		// Construction/Destruction
		c_studio_model_renderer( void );
		virtual ~c_studio_model_renderer( void ) = 0; // [ 0 ]

		// Initialization
		virtual void init( void ) = 0; // [ 1 ]

	public:
		// Public Interfaces
		virtual int studio_draw_model( int flags ) = 0; // [ 2 ]
		virtual int studio_draw_player( int flags, struct entity_state_s* pplayer ) = 0; // [ 3 ]
		virtual int studio_draw_monster( int flags, struct cl_entity_s* pmonster ) = 0; // [ 4 ]
	public:
		// Local interfaces
		//

		// Look up animation data for sequence
		virtual struct mstudioanim_t* studio_get_anim( struct model_t* m_pSubModel, struct mstudioseqdesc_t* pseqdesc ) = 0; // [ 5 ]

		// Interpolate model position and angles and set up matrices
		virtual void studio_setup_transform( int trivial_accept ) = 0; // [ 6 ]

		// Set up model bone positions
		virtual void studio_setup_bones( void ) = 0; // [ 7 ]

		// Find final attachment points
		virtual void studio_calc_attachments( void ) = 0; // [ 8 ]

		// Save bone matrices and names
		virtual void studio_save_bones( void ) = 0; // [ 9 ]

		// Merge cached bones with current bones for model
		virtual void studio_merge_bones( model_t* m_pSubModel ) = 0; // [ 10 ]

		// Determine interpolation fraction
		virtual float studio_estimate_interpolant( void ) = 0;  // [ 11 ]

		// Determine current frame for rendering
		virtual float studio_estimate_frame( mstudioseqdesc_t* pseqdesc ) = 0; // [ 12 ]

		// Determine current frame for rendering
		virtual float studio_estimate_gait_frame( mstudioseqdesc_t* pseqdesc ) = 0; // [ 13 ]

		// Apply special effects to transform matrix
		virtual void studio_fx_transform( cl_entity_t* ent, float transform[3][4] ) = 0; // [ 14 ]

		// Spherical interpolation of bones
		virtual void studio_slerp_bones( struct vec4_t q1[ ], float pos1[ ][3], vec4_t q2[ ], float pos2[ ][3], float s ) = 0; // [ 15 ]

		// Compute bone adjustments ( bone controllers )
		virtual void studio_calc_bone_adj( float dadt, float* adj, const byte* pcontroller1, const byte* pcontroller2, byte mouthopen ) = 0; // [ 16 ]

		// Get bone quaternions
		virtual void studio_calc_bone_quat( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* q ) = 0;  // [ 17 ]

		// Get bone positions
		virtual void studio_calc_bone_pos( int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos ) = 0; // [ 18 ]

		// Compute rotations
		virtual void studio_calc_rotations( float pos[ ][3], vec4_t* q, mstudioseqdesc_t* pseqdesc, mstudioanim_t* panim, float f ) = 0; // [ 19 ]

		// Send bones and verts to renderer
		virtual void studio_render_model( void ) = 0; // [ 20 ]

		// Finalize rendering
		virtual void studio_render_final( void ) = 0; // [ 21 ]

		// GL&D3D vs. Software renderer finishing functions
		virtual void studio_render_final_software( void ) = 0; // [ 22 ]
		virtual void studio_render_final_hardware( void ) = 0; // [ 23 ]

		// Player specific data
		// Determine pitch and blending amounts for players
		virtual void studio_player_blend( mstudioseqdesc_t* pseqdesc, int* pBlend, float* pPitch ) = 0; // [ 24 ]

		// Estimate gait frame for player
		virtual void studio_estimate_gait( entity_state_t* pplayer ) = 0; // [ 25 ]

		// Process movement of player
		virtual void studio_process_gait( entity_state_t* pplayer ) = 0; // [ 26 ]

	public:

		// Client clock
		double			m_clTime;
		// Old Client clock
		double			m_clOldTime;

		// Do interpolation?
		int				m_fDoInterp;
		// Do gait estimation?
		int				m_fGaitEstimation;

		// Current render frame #
		int				m_nFrameCount;

		// Cvars that studio model code needs to reference
		//
		// Use high quality models?
		cvar_t* m_pCvarHiModels;

		// Purpose?
		cvar_t* m_pCvarIndex1;
		cvar_t* m_pCvarIndex2;
		cvar_t* m_pCvarIndex3;
		cvar_t* m_pCvarIndex4;

		// Developer debug output desired?
		cvar_t* m_pCvarDeveloper;
		// Draw entities bone hit boxes, etc?
		cvar_t* m_pCvarDrawEntities;

		// The entity which we are currently rendering.
		cl_entity_t* m_pCurrentEntity;

		// The model for the entity being rendered
		model_t* m_pRenderModel;

		// Player info for current player, if drawing a player
		struct player_info_t* m_pPlayerInfo;

		// Never used, but it's neighbor of m_pPlayerInfo... split screen?
		char	unknown[4];

		// The index of the player being drawn
		int				m_nPlayerIndex;

		// The player's gait movement
		float			m_flGaitMovement;

		// Pointer to header block for studio model data
		studiohdr_t* m_pStudioHeader;

		// Pointers to current body part and submodel
		mstudiobodyparts_t* m_pBodyPart;
		mstudiomodel_t* m_pSubModel;

		// Palette substition for top and bottom of model
		int				m_nTopColor;
		int				m_nBottomColor;

		//
		// Sprite model used for drawing studio model chrome
		model_t* m_pChromeSprite;

		// Caching
		// Number of bones in bone cache
		int				m_nCachedBones;
		// Names of cached bones
		char			m_nCachedBoneNames[128][32];
		// Cached bone & light transformation matrices
		float			m_rgCachedBoneTransform[128][3][4];
		float			m_rgCachedLightTransform[128][3][4];

		// Software renderer scale factors
		float			m_fSoftwareXScale, m_fSoftwareYScale;

		// Current view vectors and render origin
		float			m_vUp[3];
		float			m_vRight[3];
		float			m_vNormal[3];

		float			m_vRenderOrigin[3];

		// Model render counters ( from engine )
		int* m_pStudioModelCount;
		int* m_pModelsDrawn;

		// Matrices
		// Model to world transformation
		float( *m_protationmatrix )[3][4];
		// Model to view transformation
		float( *m_paliastransform )[3][4];

		// Concatenated bone and light transforms
		float( *m_pbonetransform )[128][3][4];
		float( *m_plighttransform )[128][3][4];
	};

	class c_game_studio_model_renderer : public c_studio_model_renderer {
	public:
		c_game_studio_model_renderer( void );
	};
};
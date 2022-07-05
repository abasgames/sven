#pragma once

namespace sdk {
	typedef struct rect_s
	{
		int left, right, top, bottom;
	} wrect_t;
	typedef struct client_sprite_s
	{
		char szName[ 64 ];
		char szSprite[ 64 ];
		int hspr;
		int iRes;
		wrect_t rc;
	} client_sprite_t;
	
	typedef struct SCREENINFO_s
	{
		int		iSize;
		int		iWidth;
		int		iHeight;
		int		iFlags;
		int		iCharHeight;
		short	charWidths[ 256 ];
	} SCREENINFO;
	typedef struct hud_player_info_s
	{
		char* name;
		short ping;
		byte thisplayer;  // TRUE if this is the calling player

		byte spectator;
		byte packetloss;

		void* model;
		short topcolor;
		short bottomcolor;

		std::uint64_t m_nSteamID;
	} hud_player_info_t;

	typedef struct client_textmessage_s
	{
		int		effect;
		byte	r1, g1, b1, a1;		// 2 colors for effects
		byte	r2, g2, b2, a2;
		float	x;
		float	y;
		float	fadein;
		float	fadeout;
		float	holdtime;
		float	fxtime;
		const char* pName;
		const char* pMessage;
	} client_textmessage_t;

	typedef struct con_nprint_s
	{
		int		index;			// Row #
		float	time_to_live;	// # of seconds before it dissappears
		float	color[ 3 ];		// RGB colors ( 0.0 -> 1.0 scale )
	} con_nprint_t;

	typedef struct cmdalias_s
	{
		struct cmdalias_s* next;
		char	name[ 32 ];
		char* value;
	} cmdalias_t;

	typedef struct c_engine_funcs_s {
		HSPRITE( *pfnSPR_Load )			( const char* szPicName );
		int ( *pfnSPR_Frames )			( HSPRITE hPic );
		int ( *pfnSPR_Height )			( HSPRITE hPic, int frame );
		int ( *pfnSPR_Width )			( HSPRITE hPic, int frame );
		void ( *pfnSPR_Set )				( HSPRITE hPic, int r, int g, int b );
		void ( *pfnSPR_Draw )			( int frame, int x, int y, const wrect_t* prc );
		void ( *pfnSPR_DrawHoles )		( int frame, int x, int y, const wrect_t* prc );
		void ( *pfnSPR_DrawAdditive )	( int frame, int x, int y, const wrect_t* prc );
		void ( *pfnSPR_EnableScissor )	( int x, int y, int width, int height );
		void ( *pfnSPR_DisableScissor )	( void );
		client_sprite_t* ( *pfnSPR_GetList )			( char* psz, int* piCount );
		void ( *pfnFillRGBA )			( int x, int y, int width, int height, int r, int g, int b, int a );
		int	 ( *pfnGetScreenInfo ) 		( SCREENINFO* pscrinfo );
		void ( *pfnSetCrosshair )		( HSPRITE hspr, wrect_t rc, int r, int g, int b );
		struct cvar_s* ( *pfnRegisterVariable )	( const char* szName, const char* szValue, int flags );
		float						( *pfnGetCvarFloat )		( const char* szName );
		const char* ( *pfnGetCvarString )		( const char* szName );
		int	( *pfnAddCommand )			( const char* cmd_name, void ( *function )( void ) );
		int	( *pfnHookUserMsg )			( const char* szMsgName, void* pfn );
		int	( *pfnServerCmd )			( const char* szCmdString );
		int	( *pfnClientCmd )			( const char* szCmdString );
		void						( *pfnGetPlayerInfo )		( int ent_num, hud_player_info_t* pinfo );
		void						( *pfnPlaySoundByName )		( char* szSound, float volume );
		void						( *pfnPlaySoundByIndex )	( int iSound, float volume );
		void						( *pfnAngleVectors )		( const float* vecAngles, float* forward, float* right, float* up );
		client_textmessage_t* ( *pfnTextMessageGet )		( const char* pName );
		int							( *pfnDrawCharacter )		( int x, int y, int number, int r, int g, int b );
		int							( *pfnDrawConsoleString )	( int x, int y, const char* string );
		void						( *pfnDrawSetTextColor )	( float r, float g, float b );
		void						( *pfnDrawConsoleStringLen )( const char* string, int* length, int* height );
		void						( *pfnConsolePrint )		( const char* string );
		void						( *pfnCenterPrint )			( const char* string );
		int							( *GetWindowCenterX )		( void );
		int							( *GetWindowCenterY )		( void );
		void						( *GetViewAngles )			( float* );
		void						( *SetViewAngles )			( float* );
		int							( *GetMaxClients )			( void );
		void						( *Cvar_SetValue )			( const char* cvar, float value );
		int       					( *Cmd_Argc )					( void );
		char* ( *Cmd_Argv )				( int arg );
		void						( *Con_Printf )				( const char* fmt, ... );
		void						( *Con_DPrintf )			( char* fmt, ... );
		void						( *Con_NPrintf )			( int pos, char* fmt, ... );
		void						( *Con_NXPrintf )			( struct con_nprint_s* info, char* fmt, ... );
		const char* ( *PhysInfo_ValueForKey )	( const char* key );
		const char* ( *ServerInfo_ValueForKey )( const char* key );
		float						( *GetClientMaxspeed )		( void );
		int							( *CheckParm )				( char* parm, char** ppnext );
		void						( *Key_Event )				( int key, int down );
		void						( *GetMousePosition )		( int* mx, int* my );
		int							( *IsNoClipping )			( void );
		struct cl_entity_s* ( *GetLocalPlayer )		( void );
		struct cl_entity_s* ( *GetViewModel )			( void );
		struct cl_entity_s* ( *GetEntityByIndex )		( int idx );
		float						( *GetClientTime )			( void );
		void						( *V_CalcShake )			( void );
		void						( *V_ApplyShake )			( float* origin, float* angles, float factor );
		int							( *PM_PointContents )		( float* point, int* truecontents );
		int							( *PM_WaterEntity )			( float* p );
		struct pmtrace_s* ( *PM_TraceLine )			( float* start, float* end, int flags, int usehull, int ignore_pe );
		struct model_s* ( *CL_LoadModel )			( const char* modelname, int* index );
		int							( *CL_CreateVisibleEntity )	( int type, struct cl_entity_s* ent );
		const struct model_s* ( *GetSpritePointer )		( HSPRITE hSprite );
		void						( *pfnPlaySoundByNameAtLocation )	( char* szSound, float volume, float* origin );
		unsigned short				( *pfnPrecacheEvent )		( int type, const char* psz );
		void						( *pfnPlaybackEvent )		( int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
		void						( *pfnWeaponAnim )			( int iAnim, int body );
		float						( *pfnRandomFloat )			( float flLow, float flHigh );
		long						( *pfnRandomLong )			( long lLow, long lHigh );
		void						( *pfnHookEvent )			( char* name, void ( *pfnEvent )( struct event_args_s* args ) );
		int							( *Con_IsVisible )			( );
		const char* ( *pfnGetGameDirectory )	( void );
		struct cvar_s* ( *pfnGetCvarPointer )		( const char* szName );
		const char* ( *Key_LookupBinding )		( const char* pBinding );
		const char* ( *pfnGetLevelName )		( void );
		void						( *pfnGetScreenFade )		( struct screenfade_s* fade );
		void						( *pfnSetScreenFade )		( struct screenfade_s* fade );
		void* ( *VGui_GetPanel )( );
		void ( *VGui_ViewportPaintBackground ) ( int extents[ 4 ] );
		byte* ( *COM_LoadFile ) ( char* path, int usehunk, int* pLength );
		char* ( *COM_ParseFile )( char* data, char* token );
		void ( *COM_FreeFile )( void* buffer );
		struct triangleapi_s* pTriAPI;
		struct efx_api_s* pEfxAPI;
		struct event_api_s* pEventAPI;
		struct demo_api_s* pDemoAPI;
		struct net_api_s* pNetAPI;
		struct IVoiceTweak_s* pVoiceTweak;
		int ( *pfnEngSrc_IsSpectateOnly_t ) ( void );
		struct model_s* ( *pfnEngSrc_LoadMapSprite_t )( const char* filename );
		void ( *pfnEngSrc_COM_AddAppDirectoryToSearchPath_t )( const char* pszBaseDir, const char* appName );
		int	( *pfnEngSrc_COM_ExpandFilename_t )( const char* fileName, char* nameOutBuffer, int nameOutBufferSize );
		const char* ( *pfnEngSrc_PlayerInfo_ValueForKey_t )( int playerNum, const char* key );
		void ( *pfnEngSrc_PlayerInfo_SetValueForKey_t )( const char* key, const char* value );
		bool( *pfnEngSrc_GetPlayerUniqueID_t )( int iPlayer, char playerID[ 16 ] );
		int ( *pfnEngSrc_GetTrackerIDForPlayer_t )( int playerSlot );
		int ( *pfnEngSrc_GetPlayerForTrackerID_t )( int trackerID );
		int ( *pfnEngSrc_pfnServerCmdUnreliable_t )( char* szCmdString );
		void ( *pfnEngSrc_GetMousePos_t )( struct tagPOINT* ppt );
		void ( *pfnEngSrc_SetMousePos_t )( int x, int y );
		void ( *pfnEngSrc_SetMouseEnable_t )( bool fEnable );
		struct cvar_s* ( *pfnGetCvarList )( void );
		struct cmd_s* ( *pfnGetCmdList )( void );
		char* ( *pfnGetCvarName )( struct cvar_s* cvar );
		char* ( *pfnGetCmdName )( struct cmd_s* cmd );
		float ( *pfnGetServerTime )( void );
		float ( *pfnGetGravity )( void );
		const struct model_s* ( *pfnPrecacheSprite )( HSPRITE spr );
		void ( *OverrideLightmap )( int override );
		void ( *SetLightmapColor )( float r, float g, float b );
		void ( *SetLightmapDarkness )( float dark );
		int ( *pfnGetSequenceByName )( int flags, const char* seq );
		void ( *pfnSPR_DrawGeneric )( int frame, int x, int y, const wrect_t* prc, int blendsrc, int blenddst, int unknown1, int unknown2 );
		void ( *pfnLoadSentence )( void );
		int ( *pfnDrawLocalizedHudString )( int x, int y, const char* str, int r, int g, int b );
		int ( *pfnDrawLocalizedConsoleString )( int x, int y, const char* str );
		const char* ( *LocalPlayerInfo_ValueForKey )( const char* key );
		void ( *pfnDrawText_0 )( int x, int y, const char* text, unsigned long font );
		int ( *pfnDrawUnicodeCharacter )( int x, int y, short number, int r, int g, int b, unsigned long hfont );
		int ( *pfnCheckSoundFile )( const char* path );
		void* ( *GetCareerGameInterface )( void );
		void ( *pfnCvar_Set )( const char* cvar, const char* value );
		int ( *IsSinglePlayer )( void );
		void ( *pfnPlaySound )( const char* sound, float vol, float pitch );
		void ( *pfnPlayMp3 )( const char* mp3, int flags );
		float ( *Sys_FloatTime )( void );
		void ( *pfnSetArray )( int* array, int size );
		void ( *pfnSetClearArray )( int* array, int size );
		void ( *pfnClearArray )( void );
		void ( *pfnPlaySound2 )( const char* sound, float vol, float pitch );
		void ( *pfnTintRGBA )( int x, int y, int width, int height, int r, int g, int b, int a );
		void PrintConsole( const char* fmt, ... ) {
			char buffer[256];
			va_list list;
			va_start( fmt, list );
			vsprintf_s( buffer, 256, fmt, list );
			va_end( list );

			pfnConsolePrint( buffer );
		};
	}c_engine_funcs;



	typedef struct cvar_s
	{
		const char* name;
		const char* string;
		int		flags;
		float	value;
		struct cvar_s* next;
	} cvar_t;

	typedef struct event_api_s
	{
		int		version;
		void	( *EV_PlaySound ) ( int ent, float* origin, int channel, const char* sample, float volume, float attenuation, int fFlags, int pitch );
		void	( *EV_StopSound ) ( int ent, int channel, const char* sample );
		int		( *EV_FindModelIndex )( const char* pmodel );
		int		( *EV_IsLocal ) ( int playernum );
		int		( *EV_LocalPlayerDucking ) ( void );
		void	( *EV_LocalPlayerViewheight ) ( float* );
		void	( *EV_LocalPlayerBounds ) ( int hull, float* mins, float* maxs );
		int		( *EV_IndexFromTrace ) ( struct pmtrace_s* pTrace );
		struct physent_s* ( *EV_GetPhysent ) ( int idx );
		void	( *EV_SetUpPlayerPrediction ) ( int dopred, int bIncludeLocalClient );
		void	( *EV_PushPMStates ) ( void );
		void	( *EV_PopPMStates ) ( void );
		void	( *EV_SetSolidPlayers ) ( int playernum );
		void	( *EV_SetTraceHull ) ( int hull );
		void	( *EV_PlayerTrace ) ( float* start, float* end, int traceFlags, int ignore_pe, struct pmtrace_s* tr );
		void	( *EV_WeaponAnimation ) ( int sequence, int body );
		unsigned short ( *EV_PrecacheEvent ) ( int type, const char* psz );
		void	( *EV_PlaybackEvent ) ( int flags, const struct edict_s* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );
		const char* ( *EV_TraceTexture ) ( int ground, float* vstart, float* vend );
		void	( *EV_StopAllSounds ) ( int entnum, int entchannel );
		void    ( *EV_KillEvents ) ( int entnum, const char* eventname );
	} event_api_t;

	typedef struct demo_api_s
	{
		int		( *IsRecording )	( void );
		int		( *IsPlayingback )	( void );
		int		( *IsTimeDemo )		( void );
		void	( *WriteBuffer )	( int size, unsigned char* buffer );
	} demo_api_t;

	struct TEMPENTITY;
	struct BEAM;

	typedef struct
	{
		byte r, g, b;
	} color24;

	typedef struct dlight_s
	{
		sdk::c_vector origin;
		float radius;
		color24 color;
		float die;		// stop lighting after this time
		float decay;	// drop this each second
		float minlight; // don't add when contributing less
		int key;
		int dark; // subtracts light instead of adding
	} dlight_t;

	struct efx_api_s
	{
		struct particle_t* ( *R_AllocParticle )			( void ( *callback ) ( struct particle_s* particle, float frametime ) );
		void		( *R_BlobExplosion )			( float* org );
		void		( *R_Blood )					( float* org, float* dir, int pcolor, int speed );
		void		( *R_BloodSprite )				( float* org, int colorindex, int modelIndex, int modelIndex2, float size );
		void		( *R_BloodStream )				( float* org, float* dir, int pcolor, int speed );
		void		( *R_BreakModel )				( float* pos, float* size, float* dir, float random, float life, int count, int modelIndex, char flags );
		void		( *R_Bubbles )					( float* mins, float* maxs, float height, int modelIndex, int count, float speed );
		void		( *R_BubbleTrail )				( float* start, float* end, float height, int modelIndex, int count, float speed );
		void		( *R_BulletImpactParticles )	( float* pos );
		void		( *R_EntityParticles )			( struct cl_entity_s* ent );
		void		( *R_Explosion )				( float* pos, int model, float scale, float framerate, int flags );
		void		( *R_FizzEffect )				( struct cl_entity_s* pent, int modelIndex, int density );
		void		( *R_FireField ) 				( float* org, int radius, int modelIndex, int count, int flags, float life );
		void		( *R_FlickerParticles )			( float* org );
		void		( *R_FunnelSprite )				( float* org, int modelIndex, int reverse );
		void		( *R_Implosion )				( float* end, float radius, int count, float life );
		void		( *R_LargeFunnel )				( float* org, int reverse );
		void		( *R_LavaSplash )				( float* org );
		void		( *R_MultiGunshot )				( float* org, float* dir, float* noise, int count, int decalCount, int* decalIndices );
		void		( *R_MuzzleFlash )				( float* pos1, int type );
		void		( *R_ParticleBox )				( float* mins, float* maxs, unsigned char r, unsigned char g, unsigned char b, float life );
		void		( *R_ParticleBurst )			( float* pos, int size, int color, float life );
		void		( *R_ParticleExplosion )		( float* org );
		void		( *R_ParticleExplosion2 )		( float* org, int colorStart, int colorLength );
		void		( *R_ParticleLine )				( float* start, float* end, unsigned char r, unsigned char g, unsigned char b, float life );
		void		( *R_PlayerSprites )			( int client, int modelIndex, int count, int size );
		void		( *R_Projectile )				( float* origin, float* velocity, int modelIndex, int life, int owner, void ( *hitcallback )( struct tempent_s* ent, struct pmtrace_s* ptr ) );
		void		( *R_RicochetSound )			( float* pos );
		void		( *R_RicochetSprite )			( float* pos, struct model_s* pmodel, float duration, float scale );
		void		( *R_RocketFlare )				( float* pos );
		void		( *R_RocketTrail )				( float* start, float* end, int type );
		void		( *R_RunParticleEffect )		( float* org, float* dir, int color, int count );
		void		( *R_ShowLine )					( float* start, float* end );
		void		( *R_SparkEffect )				( float* pos, int count, int velocityMin, int velocityMax );
		void		( *R_SparkShower )				( float* pos );
		void		( *R_SparkStreaks )				( float* pos, int count, int velocityMin, int velocityMax );
		void		( *R_Spray )					( float* pos, float* dir, int modelIndex, int count, int speed, int spread, int rendermode );
		void		( *R_Sprite_Explode )			( TEMPENTITY* pTemp, float scale, int flags );
		void		( *R_Sprite_Smoke )				( TEMPENTITY* pTemp, float scale );
		void		( *R_Sprite_Spray )				( float* pos, float* dir, int modelIndex, int count, int speed, int iRand );
		void		( *R_Sprite_Trail )				( int type, float* start, float* end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed );
		void		( *R_Sprite_WallPuff )			( TEMPENTITY* pTemp, float scale );
		void		( *R_StreakSplash )				( float* pos, float* dir, int color, int count, float speed, int velocityMin, int velocityMax );
		void		( *R_TracerEffect )				( float* start, float* end );
		void		( *R_UserTracerParticle )		( float* org, float* vel, float life, int colorIndex, float length, unsigned char deathcontext, void ( *deathfunc )( struct particle_s* particle ) );
		particle_t* ( *R_TracerParticles )			( float* org, float* vel, float life );
		void		( *R_TeleportSplash )			( float* org );
		void		( *R_TempSphereModel )			( float* pos, float speed, float life, int count, int modelIndex );
		TEMPENTITY* ( *R_TempModel )				( float* pos, float* dir, float* angles, float life, int modelIndex, int soundtype );
		TEMPENTITY* ( *R_DefaultSprite )			( float* pos, int spriteIndex, float framerate );
		TEMPENTITY* ( *R_TempSprite )				( float* pos, float* dir, float scale, int modelIndex, int rendermode, int renderfx, float a, float life, int flags );
		int			( *Draw_DecalIndex )			( int id );
		int			( *Draw_DecalIndexFromName )	( char* name );
		void		( *R_DecalShoot )				( int textureIndex, int entity, int modelIndex, float* position, int flags );
		void		( *R_AttachTentToPlayer )		( int client, int modelIndex, float zoffset, float life );
		void		( *R_KillAttachedTents )		( int client );
		BEAM* ( *R_BeamCirclePoints )		( int type, float* start, float* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
		BEAM* ( *R_BeamEntPoint )			( int startEnt, float* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
		BEAM* ( *R_BeamEnts )				( int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
		BEAM* ( *R_BeamFollow )				( int startEnt, int modelIndex, float life, float width, float r, float g, float b, float brightness );
		void		( *R_BeamKill )					( int deadEntity );
		BEAM* ( *R_BeamLightning )			( float* start, float* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed );
		BEAM* ( *R_BeamPoints )				( float* start, float* end, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
		BEAM* ( *R_BeamRing )				( int startEnt, int endEnt, int modelIndex, float life, float width, float amplitude, float brightness, float speed, int startFrame, float framerate, float r, float g, float b );
		dlight_t* ( *CL_AllocDlight )			( int key );
		dlight_t* ( *CL_AllocElight )			( int key );
		TEMPENTITY* ( *CL_TempEntAlloc )			( float* org, struct model_s* model );
		TEMPENTITY* ( *CL_TempEntAllocNoModel )	( float* org );
		TEMPENTITY* ( *CL_TempEntAllocHigh )		( float* org, struct model_s* model );
		TEMPENTITY* ( *CL_TentEntAllocCustom )		( float* origin, struct model_s* model, int high, void ( *callback ) ( struct tempent_s* ent, float frametime, float currenttime ) );
		void		( *R_GetPackedColor )			( short* packed, short color );
		short		( *R_LookupColor )				( unsigned char r, unsigned char g, unsigned char b );
		void		( *R_DecalRemoveAll )			( int textureIndex ); 
		void		( *R_FireCustomDecal )			( int textureIndex, int entity, int modelIndex, float* position, int flags, float scale );
	};

	typedef void ( *net_api_response_func_t ) ( struct net_response_s* response );

	typedef enum
	{
		MicrophoneVolume = 0,	
		OtherSpeakerScale,		
		MicBoost,				
	} VoiceTweakControl;

	typedef struct IVoiceTweak_s
	{
		int				( *StartVoiceTweakMode )( );
		void			( *EndVoiceTweakMode )( );
		void			( *SetControlFloat )( VoiceTweakControl iControl, float value );
		float			( *GetControlFloat )( VoiceTweakControl iControl );

		int             ( *GetSpeakingVolume )( );
	} IVoiceTweak;

	typedef enum
	{
		NA_UNUSED,
		NA_LOOPBACK,
		NA_BROADCAST,
		NA_IP,
		NA_IPX,
		NA_BROADCAST_IPX,
	} netadrtype_t;

	typedef struct netadr_s
	{
		netadrtype_t	type;
		unsigned char	ip[ 4 ];
		unsigned char	ipx[ 10 ];
		unsigned short	port;
	} netadr_t;

	typedef struct net_adrlist_s
	{
		struct net_adrlist_s* next;
		netadr_t				remote_address;
	} net_adrlist_t;

	typedef struct net_response_s
	{
		int			error;
		int			context;
		int			type;
		netadr_t	remote_address;
		double		ping;
		void* response;
	} net_response_t;

	typedef struct net_status_s
	{
		int			connected;
		netadr_t	local_address;
		netadr_t	remote_address;
		int			packet_loss;
		double		latency;
		double		connection_time;
		double		rate;
	} net_status_t;

	typedef struct net_api_s
	{
		void		( *InitNetworking )( void );
		void		( *Status ) ( struct net_status_s* status );
		void		( *SendRequest ) ( int context, int request, int flags, double timeout, struct netadr_s* remote_address, net_api_response_func_t response );
		void		( *CancelRequest ) ( int context );
		void		( *CancelAllRequests ) ( void );
		char* ( *AdrToString ) ( struct netadr_s* a );
		int			( *CompareAdr ) ( struct netadr_s* a, struct netadr_s* b );
		int			( *StringToAdr ) ( char* s, struct netadr_s* a );
		const char* ( *ValueForKey ) ( const char* s, const char* key );
		void		( *RemoveKey ) ( char* s, const char* key );
		void		( *SetValueForKey ) ( char* s, const char* key, const char* value, int maxsize );
	} net_api_t;

#define TraceResult void
#define cvar_t void
#define ALERT_TYPE int
#define uint32 unsigned int
#define int32 int
#define CRC32_t void
#define PRINT_TYPE int

	typedef struct link_s
	{
		struct link_s* prev, * next;
	} link_t;
#define string_t int
#define vec3_t sdk::c_vector

	typedef struct entvars_s
	{
		string_t	classname;
		string_t	globalname;

		vec3_t		origin;
		vec3_t		oldorigin;
		vec3_t		velocity;
		vec3_t		basevelocity;
		vec3_t      clbasevelocity;  // Base velocity that was passed in to server physics so 
									 //  client can predict conveyors correctly.  Server zeroes it, so we need to store here, too.
		vec3_t		movedir;

		vec3_t		angles;			// Model angles
		vec3_t		avelocity;		// angle velocity (degrees per second)
		vec3_t		punchangle;		// auto-decaying view angle adjustment
		vec3_t		v_angle;		// Viewing angle (player only)

		// For parametric entities
		vec3_t		endpos;
		vec3_t		startpos;
		float		impacttime;
		float		starttime;

		int			fixangle;		// 0:nothing, 1:force view angles, 2:add avelocity
		float		idealpitch;
		float		pitch_speed;
		float		ideal_yaw;
		float		yaw_speed;

		int			modelindex;
		string_t	model;

		int			viewmodel;		// player's viewmodel
		int			weaponmodel;	// what other players see

		vec3_t		absmin;		// BB max translated to world coord
		vec3_t		absmax;		// BB max translated to world coord
		vec3_t		mins;		// local BB min
		vec3_t		maxs;		// local BB max
		vec3_t		size;		// maxs - mins

		float		ltime;
		float		nextthink;

		int			movetype;
		int			solid;

		int			skin;
		int			body;			// sub-model selection for studiomodels
		int 		effects;

		float		gravity;		// % of "normal" gravity
		float		friction;		// inverse elasticity of MOVETYPE_BOUNCE

		int			light_level;

		int			sequence;		// animation sequence
		int			gaitsequence;	// movement animation sequence for player (0 for none)
		float		frame;			// % playback position in animation sequences (0..255)
		float		animtime;		// world time when frame was set
		float		framerate;		// animation playback rate (-8x to 8x)
		byte		controller[4];	// bone controller setting (0..255)
		byte		blending[2];	// blending amount between sub-sequences (0..255)

		float		scale;			// sprite rendering scale (0..255)

		int			rendermode;
		float		renderamt;
		vec3_t		rendercolor;
		int			renderfx;

		float		health;
		float		frags;
		int			weapons;  // bit mask for available weapons
		float		takedamage;

		int			deadflag;
		vec3_t		view_ofs;	// eye position

		int			button;
		int			impulse;

		struct edict_t* chain;			// Entity pointer when linked into a linked list
		struct edict_t* dmg_inflictor;
		struct edict_t* enemy;
		struct edict_t* aiment;		// entity pointer when MOVETYPE_FOLLOW
		struct edict_t* owner;
		struct edict_t* groundentity;

		int			spawnflags;
		int			flags;

		int			colormap;		// lowbyte topcolor, highbyte bottomcolor
		int			team;

		float		max_health;
		float		teleport_time;
		float		armortype;
		float		armorvalue;
		int			waterlevel;
		int			watertype;

		string_t	target;
		string_t	targetname;
		string_t	netname;
		string_t	message;

		float		dmg_take;
		float		dmg_save;
		float		dmg;
		float		dmgtime;

		string_t	noise;
		string_t	noise1;
		string_t	noise2;
		string_t	noise3;

		float		speed;
		float		air_finished;
		float		pain_finished;
		float		radsuit_finished;

		edict_t* pContainingEntity;

		int			playerclass;
		float		maxspeed;

		float		fov;
		int			weaponanim;

		int			pushmsec;

		int			bInDuck;
		int			flTimeStepSound;
		int			flSwimTime;
		int			flDuckTime;
		int			iStepLeft;
		float		flFallVelocity;

		int			gamestate;

		int			oldbuttons;

		int			groupinfo;

		// For mods
		int			iuser1;
		int			iuser2;
		int			iuser3;
		int			iuser4;
		float		fuser1;
		float		fuser2;
		float		fuser3;
		float		fuser4;
		vec3_t		vuser1;
		vec3_t		vuser2;
		vec3_t		vuser3;
		vec3_t		vuser4;
		struct edict_t* euser1;
		struct edict_t* euser2;
		struct edict_t* euser3;
		struct edict_t* euser4;
	} entvars_t;

	struct edict_t
	{
		int	free;
		int			serialnumber;
		link_t		area;				// linked to a division node or leaf

		int			headnode;			// -1 to use normal leaf check
		int			num_leafs;
		short		leafnums[48];

		float		freetime;			// sv.time when the object was freed

		void* pvPrivateData;		// Alloced and freed by engine, used by DLLs

		entvars_t	v;					// C exported fields from progs
		// other fields from progs come immediately after
	};

	typedef struct enginefuncs_s
	{
		int			( *pfnPrecacheModel )			( char* s );
		int			( *pfnPrecacheSound )			( char* s );
		void		( *pfnSetModel )				( edict_t* e, const char* m );
		int			( *pfnModelIndex )			( const char* m );
		int			( *pfnModelFrames )			( int modelIndex );
		void		( *pfnSetSize )				( edict_t* e, const float* rgflMin, const float* rgflMax );
		void		( *pfnChangeLevel )			( char* s1, char* s2 );
		void		( *pfnGetSpawnParms )			( edict_t* ent );
		void		( *pfnSaveSpawnParms )		( edict_t* ent );
		float		( *pfnVecToYaw )				( const float* rgflVector );
		void		( *pfnVecToAngles )			( const float* rgflVectorIn, float* rgflVectorOut );
		void		( *pfnMoveToOrigin )			( edict_t* ent, const float* pflGoal, float dist, int iMoveType );
		void		( *pfnChangeYaw )				( edict_t* ent );
		void		( *pfnChangePitch )			( edict_t* ent );
		edict_t* ( *pfnFindEntityByString )	( edict_t* pEdictStartSearchAfter, const char* pszField, const char* pszValue );
		int			( *pfnGetEntityIllum )		( edict_t* pEnt );
		edict_t* ( *pfnFindEntityInSphere )	( edict_t* pEdictStartSearchAfter, const float* org, float rad );
		edict_t* ( *pfnFindClientInPVS )		( edict_t* pEdict );
		edict_t* ( *pfnEntitiesInPVS )			( edict_t* pplayer );
		void		( *pfnMakeVectors )			( const float* rgflVector );
		void		( *pfnAngleVectors )			( const float* rgflVector, float* forward, float* right, float* up );
		edict_t* ( *pfnCreateEntity )			( void );
		void		( *pfnRemoveEntity )			( edict_t* e );
		edict_t* ( *pfnCreateNamedEntity )		( int className );
		void		( *pfnMakeStatic )			( edict_t* ent );
		int			( *pfnEntIsOnFloor )			( edict_t* e );
		int			( *pfnDropToFloor )			( edict_t* e );
		int			( *pfnWalkMove )				( edict_t* ent, float yaw, float dist, int iMode );
		void		( *pfnSetOrigin )				( edict_t* e, const float* rgflOrigin );
		void		( *pfnEmitSound )				( edict_t* entity, int channel, const char* sample, /*int*/float volume, float attenuation, int fFlags, int pitch );
		void		( *pfnEmitAmbientSound )		( edict_t* entity, float* pos, const char* samp, float vol, float attenuation, int fFlags, int pitch );
		void		( *pfnTraceLine )				( const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr );
		void		( *pfnTraceToss )				( edict_t* pent, edict_t* pentToIgnore, TraceResult* ptr );
		int			( *pfnTraceMonsterHull )		( edict_t* pEdict, const float* v1, const float* v2, int fNoMonsters, edict_t* pentToSkip, TraceResult* ptr );
		void		( *pfnTraceHull )				( const float* v1, const float* v2, int fNoMonsters, int hullNumber, edict_t* pentToSkip, TraceResult* ptr );
		void		( *pfnTraceModel )			( const float* v1, const float* v2, int hullNumber, edict_t* pent, TraceResult* ptr );
		const char* ( *pfnTraceTexture )			( edict_t* pTextureEntity, const float* v1, const float* v2 );
		void		( *pfnTraceSphere )			( const float* v1, const float* v2, int fNoMonsters, float radius, edict_t* pentToSkip, TraceResult* ptr );
		void		( *pfnGetAimVector )			( edict_t* ent, float speed, float* rgflReturn );
		void		( *pfnServerCommand )			( char* str );
		void		( *pfnServerExecute )			( void );
		void		( *pfnClientCommand )			( edict_t* pEdict, char* szFmt, ... );
		void		( *pfnParticleEffect )		( const float* org, const float* dir, float color, float count );
		void		( *pfnLightStyle )			( int style, char* val );
		int			( *pfnDecalIndex )			( const char* name );
		int			( *pfnPointContents )			( const float* rgflVector );
		void		( *pfnMessageBegin )			( int msg_dest, int msg_type, const float* pOrigin, edict_t* ed );
		void		( *pfnMessageEnd )			( void );
		void		( *pfnWriteByte )				( int iValue );
		void		( *pfnWriteChar )				( int iValue );
		void		( *pfnWriteShort )			( int iValue );
		void		( *pfnWriteLong )				( int iValue );
		void		( *pfnWriteAngle )			( float flValue );
		void		( *pfnWriteCoord )			( float flValue );
		void		( *pfnWriteString )			( const char* sz );
		void		( *pfnWriteEntity )			( int iValue );
		void		( *pfnCVarRegister )			( cvar_t* pCvar );
		float		( *pfnCVarGetFloat )			( const char* szVarName );
		const char* ( *pfnCVarGetString )			( const char* szVarName );
		void		( *pfnCVarSetFloat )			( const char* szVarName, float flValue );
		void		( *pfnCVarSetString )			( const char* szVarName, const char* szValue );
		void		( *pfnAlertMessage )			( ALERT_TYPE atype, char* szFmt, ... );
		void		( *pfnEngineFprintf )			( void* pfile, char* szFmt, ... );
		void* ( *pfnPvAllocEntPrivateData )	( edict_t* pEdict, int32 cb );
		void* ( *pfnPvEntPrivateData )		( edict_t* pEdict );
		void		( *pfnFreeEntPrivateData )	( edict_t* pEdict );
		const char* ( *pfnSzFromIndex )			( int iString );
		int			( *pfnAllocString )			( const char* szValue );
		struct entvars_s* ( *pfnGetVarsOfEnt )			( edict_t* pEdict );
		edict_t* ( *pfnPEntityOfEntOffset )	( int iEntOffset );
		int			( *pfnEntOffsetOfPEntity )	( const edict_t* pEdict );
		int			( *pfnIndexOfEdict )			( const edict_t* pEdict );
		edict_t* ( *pfnPEntityOfEntIndex )		( int iEntIndex );
		edict_t* ( *pfnFindEntityByVars )		( struct entvars_s* pvars );
		void* ( *pfnGetModelPtr )			( edict_t* pEdict );
		int			( *pfnRegUserMsg )			( const char* pszName, int iSize );
		void		( *pfnAnimationAutomove )		( const edict_t* pEdict, float flTime );
		void		( *pfnGetBonePosition )		( const edict_t* pEdict, int iBone, float* rgflOrigin, float* rgflAngles );
		uint32( *pfnFunctionFromName )	( const char* pName );
		const char* ( *pfnNameForFunction )		( uint32 function );
		void		( *pfnClientPrintf )			( edict_t* pEdict, PRINT_TYPE ptype, const char* szMsg ); // JOHN: engine callbacks so game DLL can print messages to individual clients
		void		( *pfnServerPrint )			( const char* szMsg );
		const char* ( *pfnCmd_Args )				( void );		// these 3 added 
		const char* ( *pfnCmd_Argv )				( int argc );	// so game DLL can easily 
		int			( *pfnCmd_Argc )				( void );		// access client 'cmd' strings
		void		( *pfnGetAttachment )			( const edict_t* pEdict, int iAttachment, float* rgflOrigin, float* rgflAngles );
		void		( *pfnCRC32_Init )			( CRC32_t* pulCRC );
		void        ( *pfnCRC32_ProcessBuffer )   ( CRC32_t* pulCRC, void* p, int len );
		void		( *pfnCRC32_ProcessByte )     ( CRC32_t* pulCRC, unsigned char ch );
		CRC32_t( *pfnCRC32_Final )			( CRC32_t* pulCRC );
		int32( *pfnRandomLong )			( int32  lLow, int32  lHigh );
		float		( *pfnRandomFloat )			( float flLow, float flHigh );
		void		( *pfnSetView )				( const edict_t* pClient, const edict_t* pViewent );
		float		( *pfnTime )					( void );
		void		( *pfnCrosshairAngle )		( const edict_t* pClient, float pitch, float yaw );
		byte* ( *pfnLoadFileForMe )         ( char* filename, int* pLength );
		void        ( *pfnFreeFile )              ( void* buffer );
		void        ( *pfnEndSection )            ( const char* pszSectionName ); // trigger_endsection
		int 		( *pfnCompareFileTime )       ( char* filename1, char* filename2, int* iCompare );
		void        ( *pfnGetGameDir )            ( char* szGetGameDir );
		void		( *pfnCvar_RegisterVariable ) ( cvar_t* variable );
		void        ( *pfnFadeClientVolume )      ( const edict_t* pEdict, int fadePercent, int fadeOutSeconds, int holdTime, int fadeInSeconds );
		void        ( *pfnSetClientMaxspeed )     ( const edict_t* pEdict, float fNewMaxspeed );
		edict_t* ( *pfnCreateFakeClient )		( const char* netname );	// returns NULL if fake client can't be created
		void		( *pfnRunPlayerMove )			( edict_t* fakeclient, const float* viewangles, float forwardmove, float sidemove, float upmove, unsigned short buttons, byte impulse, byte msec );
		int			( *pfnNumberOfEntities )		( void );
		char* ( *pfnGetInfoKeyBuffer )		( edict_t* e );	// passing in NULL gets the serverinfo
		char* ( *pfnInfoKeyValue )			( char* infobuffer, char* key );
		void		( *pfnSetKeyValue )			( char* infobuffer, char* key, char* value );
		void		( *pfnSetClientKeyValue )		( int clientIndex, char* infobuffer, char* key, char* value );
		int			( *pfnIsMapValid )			( char* filename );
		void		( *pfnStaticDecal )			( const float* origin, int decalIndex, int entityIndex, int modelIndex );
		int			( *pfnPrecacheGeneric )		( char* s );
		int			( *pfnGetPlayerUserId )		( edict_t* e ); // returns the server assigned userid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients
		void		( *pfnBuildSoundMsg )			( edict_t* entity, int channel, const char* sample, /*int*/float volume, float attenuation, int fFlags, int pitch, int msg_dest, int msg_type, const float* pOrigin, edict_t* ed );
		int			( *pfnIsDedicatedServer )		( void );// is this a dedicated server?
		cvar_t* ( *pfnCVarGetPointer )		( const char* szVarName );
		unsigned int ( *pfnGetPlayerWONId )		( edict_t* e ); // returns the server assigned WONid for this player.  useful for logging frags, etc.  returns -1 if the edict couldn't be found in the list of clients

		// YWB 8/1/99 TFF Physics additions
		void		( *pfnInfo_RemoveKey )		( char* s, const char* key );
		const char* ( *pfnGetPhysicsKeyValue )	( const edict_t* pClient, const char* key );
		void		( *pfnSetPhysicsKeyValue )	( const edict_t* pClient, const char* key, const char* value );
		const char* ( *pfnGetPhysicsInfoString )	( const edict_t* pClient );
		unsigned short ( *pfnPrecacheEvent )		( int type, const char* psz );
		void		( *pfnPlaybackEvent )			( int flags, const edict_t* pInvoker, unsigned short eventindex, float delay, float* origin, float* angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 );

		unsigned char* ( *pfnSetFatPVS )			( float* org );
		unsigned char* ( *pfnSetFatPAS )			( float* org );

		int			( *pfnCheckVisibility )		( const edict_t* entity, unsigned char* pset );

		void		( *pfnDeltaSetField )			( struct delta_s* pFields, const char* fieldname );
		void		( *pfnDeltaUnsetField )		( struct delta_s* pFields, const char* fieldname );
		void		( *pfnDeltaAddEncoder )		( char* name, void ( *conditionalencode )( struct delta_s* pFields, const unsigned char* from, const unsigned char* to ) );
		int			( *pfnGetCurrentPlayer )		( void );
		int			( *pfnCanSkipPlayer )			( const edict_t* player );
		int			( *pfnDeltaFindField )		( struct delta_s* pFields, const char* fieldname );
		void		( *pfnDeltaSetFieldByIndex )	( struct delta_s* pFields, int fieldNumber );
		void		( *pfnDeltaUnsetFieldByIndex )( struct delta_s* pFields, int fieldNumber );

		void		( *pfnSetGroupMask )			( int mask, int op );

		int			( *pfnCreateInstancedBaseline ) ( int classname, struct entity_state_s* baseline );
		void		( *pfnCvar_DirectSet )		( struct cvar_s* var, char* value );
	} enginefuncs_t;
};
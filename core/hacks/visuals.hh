#pragma once
#include <interfaces/interfaces.hh>
#include <hacks/features.hh>
#include <utils/utils.hh>
#include <imgui/imgui.h>
#include <renderer/renderer.hh>
#include <format>
#include <codecvt>
#include <unordered_map>

struct box2d_t { float x, y, w, h; };

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

struct bonedata_t {
	sdk::c_vector bone[ 128 ];
	int parent[ 128 ];
	int flags[ 128 ];
};

bonedata_t bones[ 1024 ];

float get_entity_health( int index ) {
	return *reinterpret_cast< float* >( ( std::uintptr_t )xti::g_playerextrainfo + sizeof( extra_player_info_t ) * index + 0x42 );
};

std::uintptr_t* get_entity_player_info( int index ) {
	return reinterpret_cast< std::uintptr_t* >( ( std::uintptr_t )xti::g_playerextrainfo + sizeof( extra_player_info_t ) * index );
};

bool draw_player_box( sdk::cl_entity_t* ent, box2d_t& box ) {
	sdk::c_vector origin = ent->origin + ( ent->curstate.origin - ent->prevstate.origin ) * ent->curstate.velocity,
		min = ent->curstate.mins + origin,
		max = ent->curstate.maxs + origin;

	sdk::c_vector points[ ] = {
			sdk::c_vector( min.x, min.y, min.z ),
			sdk::c_vector( min.x, max.y, min.z ),
			sdk::c_vector( max.x, max.y, min.z ),
			sdk::c_vector( max.x, min.y, min.z ),
			sdk::c_vector( max.x, max.y, max.z ),
			sdk::c_vector( min.x, max.y, max.z ),
			sdk::c_vector( min.x, min.y, max.z ),
			sdk::c_vector( max.x, min.y, max.z )
	};

	sdk::c_vector pos = origin,
		flb, brt, blb, frt, frb, brb, blt, flt;

	if ( !xtu::world_to_screen( points[ 3 ], flb ) || !xtu::world_to_screen( points[ 5 ], brt )
		|| !xtu::world_to_screen( points[ 0 ], blb ) || !xtu::world_to_screen( points[ 4 ], frt )
		|| !xtu::world_to_screen( points[ 2 ], frb ) || !xtu::world_to_screen( points[ 1 ], brb )
		|| !xtu::world_to_screen( points[ 6 ], blt ) || !xtu::world_to_screen( points[ 7 ], flt ) )
		return false;

	sdk::c_vector arr[ 8 ] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;        // left
	float top = flb.y;        // top
	float right = flb.x;    // right
	float bottom = flb.y;    // bottom

	for ( int i = 1; i < 8; i++ )
	{
		if ( left > arr[ i ].x )
			left = arr[ i ].x;
		if ( bottom < arr[ i ].y )
			bottom = arr[ i ].y;
		if ( right < arr[ i ].x )
			right = arr[ i ].x;
		if ( top > arr[ i ].y )
			top = arr[ i ].y;
	}

	box.x = left;
	box.y = top;
	box.w = right - left;
	box.h = bottom - top;

	return true;
};

// this is so fucking ugly, but this is the only solution i could find.
// Basically, the animtime stops if the entity is dead or has frozen,
// GetClientTime ticks at the same speed as animtime, but considering that clienttime is always ticking, we
// can use this to make a dormancy check.
constexpr bool dormant( sdk::cl_entity_s* entity, sdk::cl_entity_s* local ) {
	return ( entity->curstate.messagenum < local->curstate.messagenum );
};

constexpr bool stopped_animation( sdk::cl_entity_s* entity ) {
	return ( xti::g_engine->GetClientTime( ) > entity->curstate.animtime + 0.18f );
};

using bonetransform_matrix = float[ 128 ][ 3 ][ 4 ];

class c_entity_class {
public:
	c_entity_class( sdk::model_s* model ) {
		if ( _model == nullptr )
			_model = model;
	};

	const char* get_model_name( ) {
		std::string_view model_data( _model->name );
		if ( model_data.ends_with( ".mdl" ) ) {
			auto find_slash = model_data.find_last_of( "/" );
			if ( find_slash != std::string::npos )
				model_data = model_data.substr( find_slash + 1 );
		};
		return model_data.data( );
	};

	constexpr bool is_friendly( ) const {
		return classflags & ( 1 << 0 );
	};

	constexpr bool is_dead( ) const {
		return classflags & ( 1 << 4 );
	};

	sdk::studiohdr_t* get( ) {
		return xti::g_studiomodel->Mod_Extradata( _model );
	};
private:
	sdk::model_s* _model;
	int classflags;
	int classid;
};

// as good as draw_player_box is, it costs very much performance due to the reason that there's 8 worldtoscreen calls per entity.
bool draw_entity_box( sdk::cl_entity_s* ent, box2d_t& box ) {
	if ( !ent || !ent->model )
		return false;

	auto mins = ent->origin + ( ent->curstate.origin - ent->prevstate.origin ) * ent->curstate.velocity;
	mins.z += ent->curstate.mins.z;

	auto maxs = ent->origin + ( ent->curstate.origin - ent->prevstate.origin ) * ent->curstate.velocity;
	maxs.z += ent->curstate.maxs.z;
	sdk::c_vector min, max;
	if ( !xtu::world_to_screen( mins, min ) || !xtu::world_to_screen( maxs, max ) )
		return false;

	auto height = std::fabsf( max.y - min.y );
	auto width = ( height / 4.5f ) * 2.0f;
	box.x = min.x - ( width / 2.0f );
	box.y = max.y;
	box.w = width;
	box.h = height;
	return true;
};

sdk::c_vector get_bone_location( bonetransform_matrix* matrix, int boneid ) {
	return sdk::c_vector( ( *matrix )[ boneid ][ 0 ][ 3 ], ( *matrix )[ boneid ][ 1 ][ 3 ], ( *matrix )[ boneid ][ 2 ][ 3 ] );
};

void store_bones( sdk::cl_entity_t* entity, int id ) {
	auto localplayer = xti::g_engine->GetLocalPlayer( );
	if ( !localplayer )
		return;

	if ( !entity )
		return;

	if ( !entity->model || entity == localplayer || entity->curstate.solid != 3 )
		return;

	auto studiohdr = xti::g_studiomodel->Mod_Extradata( entity->model );
	if ( !studiohdr )
		return;

	if ( studiohdr->numhitboxes == 0 || studiohdr->numbones < 1 ) // additional sanity checks
		return;

	sdk::mstudiobone_t* pbone = ( sdk::mstudiobone_t* )( ( byte* )studiohdr + studiohdr->boneindex );
	if ( !pbone )
		return;

	for ( int i = 0; i < studiohdr->numbones; i++ ) {
		bones[ id ].bone[ i ] = get_bone_location( xti::g_studiomodelrenderer->m_pbonetransform, i );
		bones[ id ].parent[ i ] = pbone[ i ].parent;
		bones[ id ].flags[ i ] = pbone[ i ].flags;
	};
}

void draw_skeleton( sdk::cl_entity_t* entity, int id ) {
	auto model = entity->model;
	if ( !model )
		return;

	auto studiohdr = xti::g_studiomodel->Mod_Extradata( model );
	if ( !studiohdr )
		return;

	if ( studiohdr->numhitboxes == 0 || studiohdr->numbones < 1 )
		return;

	for ( int i = 0; i < studiohdr->numbones; i++ ) {
		if ( bones[ id ].parent[ i ] == i || bones[ id ].parent[ i ] == -1 )
			continue;

		sdk::c_vector source_from = bones[ id ].bone[ i ];
		sdk::c_vector source_to = bones[ id ].bone[ bones[ id ].parent[ i ] ];

		sdk::c_vector dest_from, dest_to;

		if ( xtu::world_to_screen( source_from, dest_from ) && xtu::world_to_screen( source_to, dest_to ) ) {
			//Renderer::AddTextShadow( { dest_from.x, dest_from.y }, ImColor( 255, 255, 255, 255 ), std::format( "{}", i ).c_str( ) );
			Renderer::AddLine( { dest_from.x, dest_from.y }, { dest_to.x, dest_to.y }, ImColor( 255, 255, 255, 255 ) );
		};
	};
};

void healthbar( float health, box2d_t box ) {

	int g = health * 2.55;
	int r = 255 - g;
	int b = 0;

	auto hw = ( ( ( box.h ) * health ) / 100.0f );

	auto box_x = box.x - 6;
	Renderer::AddRectFilled( { box_x, box.y }, { 4, box.h }, ImColor( 25, 25, 25, 150 ) );
	Renderer::AddRectFilled( { box_x + 1, box.y + box.h - hw }, { 2, hw }, ImColor( r, g, b, 255 ) );
};

bool is_visible( sdk::cl_entity_t* ent ) {
	auto start_offset = xti::g_playermove->angles + xti::g_playermove->origin;
	auto end_offset = ent->origin;

	sdk::pmtrace_t tr;
	xti::g_engine->pEventAPI->EV_SetTraceHull( 2 );
	xti::g_engine->pEventAPI->EV_PlayerTrace( start_offset, end_offset, PM_WORLD_ONLY, -1, &tr );

	return ( tr.ent && tr.fraction >= 0.99f );
};

namespace svc {
	enum svc_entities : short {
		weapon_min,
		weapon_crowbar,
		weapon_handgun,
		weapon_revolver,
		weapon_mp5,
		weapon_shotgun,
		weapon_crossbow,
		weapon_rpg,
		weapon_gauss,
		weapon_egon,
		weapon_hgun,
		weapon_max,

		ammo_min,
		ammo_medkit, // w_medkit.mdl, m_pmedkit.mdl. uses the same model, but pmedkit comes from the player, and not the world.
		ammo_grenade,
		ammo_tripmine,
		ammo_satchel, // this could... very much be a weapon, but it's droppable & stackable, so im putting it as ammo.
		ammo_snark,
		ammo_battery,
		ammo_9mm,
		ammo_revolver,
		ammo_mp5,
		ammo_shotgun,
		ammo_crossbow,
		ammo_rpg,
		ammo_gauss,
		ammo_egon,
		ammo_mp5_grenade,
		ammo_max,

		misc_min,
		misc_grenade,
		misc_respawn,
		misc_longjump,
		misc_max,

		monster_min,
		monster_headcrab,
		monster_zombie,
		monster_houndeye,
		monster_islave,
		monster_bullsquid,
		monster_barnacle,
		monster_miniturret,
		monster_sentry,
		monster_hgrunt,
		// opposing force
		monster_strooper,
		monster_pitdrone,
		monster_massn,
		monster_voltigore,
		monster_max,

		npc_min,
		npc_scientist,
		npc_barney,
		npc_gman,
		npc_max,

		entity_unknown
	};

	std::unordered_map< std::string, svc_entities > entity_map = {
		// weapons
		{ "valve/models/w_crowbar.mdl", weapon_crowbar },
		{ "valve/models/w_9mmhandgun.mdl", weapon_handgun },
		{ "valve/models/w_357.mdl", weapon_revolver },
		{ "valve/models/w_mp5.mdl", weapon_mp5 },
		{ "valve/models/w_shotgun.mdl", weapon_shotgun },
		{ "valve/models/w_crossbow.mdl", weapon_crossbow },
		{ "valve/models/w_rpg.mdl", weapon_rpg },
		{ "valve/models/w_gauss.mdl", weapon_gauss },
		{ "valve/models/w_egon.mdl", weapon_egon },
		{ "valve/models/w_hgun.mdl", weapon_hgun }, // hgun stands for hornet gun.

		// ammo types that has backslashes.
		{ "valve/models/w_medkit.mdl", ammo_medkit }, // w_medkit (belongs to world)
		{ "valve/models/w_satchel.mdl", ammo_satchel },
		{ "valve/models/w_SQKNEST.mdl", ammo_snark },
		{ "valve/models/w_battery.mdl", ammo_battery },
		{ "valve/models/w_9mmclip.mdl", ammo_9mm },
		{ "valve/models/w_357ammobox.mdl", ammo_revolver },
		{ "valve/models/w_9mmARclip.mdl", ammo_mp5 },
		{ "valve/models/w_shotbox.mdl", ammo_shotgun },
		{ "valve/models/w_rpgammo.mdl", ammo_rpg },
		{ "valve/models/w_ARgrenade.mdl", ammo_mp5_grenade },

		// ammo models that doesnt have the backslashes in them.
		{ "w_pmedkit.mdl", ammo_medkit }, // w_pmedkit (belongs to player)
		{ "w_tripmine.mdl", ammo_tripmine },

		// ammo with weird.. long names.
		{ "E:\\projects\\svencoop\\grenade\\w_grenade.mdl", ammo_grenade },
		{ "valve\\models\\w_crossbow_clip.mdl", ammo_crossbow },
		{ "valve\\models\\w_gaussammo.mdl", ammo_gauss },


		// misc items
		{ "valve/models/grenade.mdl", misc_grenade },
		{ "lambda.mdl", misc_respawn },
		{ "valve/models/w_longjump.mdl", misc_longjump },

		// monsters
		{ "valve/models/headcrab.mdl", monster_headcrab },
		{ "valve/models/zombie.mdl", monster_zombie },
		{ "valve/models/houndeye.mdl", monster_houndeye },
		{ "valve/models/islave.mdl", monster_islave },
		{ "valve/models/bullsquid.mdl", monster_bullsquid },
		{ "valve/models/barnacle.mdl", monster_barnacle },
		{ "valve/models/miniturret.mdl", monster_miniturret },
		{ "valve/models/Sentry.mdl", monster_sentry },
		{ "hgrunt.mdl", monster_hgrunt },

		// opposing force monsters
		{ "C:\\sierra\\half-life\\gearbox\\models\\strooper.mdl", monster_strooper },
		{ "c:\\sierra\\half-life\\gearbox\\models\\Massn.mdl", monster_massn },
		{ "voltigore.mdl", monster_voltigore },

		{ "scientist.mdl", npc_scientist },
		{ "barney.mdl", npc_barney },
		{ "valve/models/gman.mdl", npc_gman }
	};

	const char* UTIL_GetEntityTypeFromModel( sdk::model_s* model ) {
		if ( !model )
			return "null";

		auto studiohdr = xti::g_studiomodel->Mod_Extradata( model );
		if ( !studiohdr || !studiohdr->name )
			return "null";

		return studiohdr->name;
	};

	short UTIL_GetEntityType( sdk::model_s* mod ) {
		if ( !mod )
			return svc::entity_unknown;

		auto studiohdr = xti::g_studiomodel->Mod_Extradata( mod );
		if ( !studiohdr )
			return svc::entity_unknown;

		if ( svc::entity_map.find( studiohdr->name ) != svc::entity_map.end( ) )
			return svc::entity_map[ studiohdr->name ];

		return svc::entity_unknown;
	};

	const char* UTIL_GetEntityName( sdk::model_s* model ) {
		short index = UTIL_GetEntityType( model );
		switch ( index ) {
		case weapon_crowbar:
			return "crowbar";
		case weapon_handgun:
			return "pistol";
		case weapon_revolver:
			return "revolver";
		case weapon_mp5:
			return "mp5";
		case weapon_shotgun:
			return "shotgun";
		case weapon_crossbow:
			return "crossbow";
		case weapon_rpg:
			return "rpg";
		case weapon_gauss:
			return "gauss";
		case weapon_egon:
			return "egon";
		case weapon_hgun:
			return "hornet gun";

		case ammo_medkit:
			return "medkit";
		case ammo_grenade:
			return "grenade";
		case ammo_tripmine:
			return "trip mine";
		case ammo_satchel:
			return "satchel";
		case ammo_snark:
			return "snark";
		case ammo_battery:
			return "battery";
		case ammo_9mm:
			return "pistol ammo";
		case ammo_revolver:
			return "revolver ammo";
		case ammo_mp5:
			return "mp5 ammo";
		case ammo_shotgun:
			return "shotgun ammo";
		case ammo_crossbow:
			return "crossbow ammo";
		case ammo_rpg:
			return "rpg ammo";
		case ammo_gauss:
			return "gauss/egon ammo";
		case ammo_mp5_grenade:
			return "mp5 grenade ammo";

		case misc_grenade:
			return "grenade boost";
		case misc_respawn:
			return "respawn";
		case misc_longjump:
			return "longjump";

		case monster_headcrab:
			return "head crab";
		case monster_zombie:
			return "zombie";
		case monster_houndeye:
			return "houndeye";
		case monster_islave:
			return "slave";
		case monster_bullsquid:
			return "bullsquid";
		case monster_barnacle:
			return "barnacle";
		case monster_miniturret:
			return "mini turret";
		case monster_sentry:
			return "sentry";
		case monster_hgrunt:
			return "human grunt";
		case monster_strooper:
			return "strooper";
		case monster_massn:
			return "massn";
		case monster_voltigore:
			return "voltigore";

		case npc_scientist:
			return "scientist";
		case npc_barney:
			return "barney";
		case npc_gman:
			return "gman";
		default:
			return UTIL_GetEntityTypeFromModel( model );
		};
	};
};


void RenderAny( sdk::cl_entity_s* entity ) {
	sdk::c_vector origin = entity->origin;
	sdk::c_vector out;

	if ( !entity || !entity->model || !entity->model->name )
		return;

	auto studiohdr = xti::g_studiomodel->Mod_Extradata( entity->model );
	if ( !studiohdr || studiohdr->numhitboxes == 0 )
		return;

	auto index = svc::UTIL_GetEntityType( entity->model );

	if ( index == svc::entity_unknown && !options::visuals::debug )
		return;

	if ( index > svc::npc_min && index < svc::npc_max || index > svc::monster_min && index < svc::monster_max )
		if ( stopped_animation( entity ) )
			return;


	box2d_t box;
	if ( !draw_player_box( entity, box ) )
		return;

	auto color = [ index ]( ) {
		if ( index > svc::weapon_min && index < svc::weapon_max )
			return ImColor( 223, 174, 83, 255 );
		else if ( index > svc::ammo_min && index < svc::ammo_max )
			return ImColor( 67, 168, 214, 255 );
		else if ( index > svc::misc_min && index < svc::misc_max )
			return ImColor( 226, 96, 233, 255 );
		else if ( index > svc::monster_min && index < svc::monster_max )
			return ImColor( 214, 67, 77, 255 );
		else if ( index > svc::npc_min && index < svc::npc_max )
			return ImColor( 67, 214, 128, 255 );
		else return ImColor( 255, 255, 255, 255 );
	};
	Renderer::AddTextShadow( { box.x + box.w / 2.0f, box.y + box.h / 2.0f }, color( ), svc::UTIL_GetEntityName( entity->model ), true );

	if( index > svc::npc_min && index < svc::npc_max || index > svc::monster_min && index < svc::monster_max )
		Renderer::AddRect( { box.x, box.y }, { box.w, box.h }, color( ) );
};

void RenderPlayer( sdk::cl_entity_s* entity, int idx ) {
	box2d_t box;
	if ( !draw_player_box( entity, box ) )
		return;

	auto player_info = xti::g_studiomodel->PlayerInfo( entity->index - 1 );
	if ( !player_info || !player_info->name || std::strlen( player_info->name ) < 1 )
		return;

	if ( get_entity_health( idx ) < 1.0f )
		return;

	if ( options::visuals::names ) {
		Renderer::AddTextShadow( { box.x + box.w / 2.0f, box.y - 8.0f }, ImColor( 255, 255, 255, 255 ), player_info->name, true );
		Renderer::AddTextShadow( { box.x + box.w / 2.0f, box.y + box.h + 8.0f }, ImColor( 255, 255, 255, 255 ), player_info->model, true );
	}
	if ( options::visuals::box )
		Renderer::AddRectOutline( { box.x, box.y }, { box.w, box.h }, ImColor( 70, 186, 128, 255 ) );

	if ( options::visuals::healthbar )
		healthbar( get_entity_health( idx ), box );
};

namespace visuals {

	void on_visuals_esp( ) {
		auto localplayer = xti::g_engine->GetLocalPlayer( );
		if ( !localplayer )
			return;

		for ( int i = 0; i < 4096; i++ ) {
			auto entity = xti::g_engine->GetEntityByIndex( i );
			if ( !entity )
				continue;

			if ( !entity->model || entity == localplayer )
				continue;

			if ( !entity->model->name || !std::strstr( entity->model->name, ".mdl" ) )
				continue;

			if ( dormant( entity, localplayer ) )
				continue;

			if ( entity->player ) {
				RenderPlayer( entity, i );
			}
			else {
				RenderAny( entity );
			};
		};
	};
};
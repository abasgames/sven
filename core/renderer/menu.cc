#include "menu.hh"
#include <imgui/imgui.h>
#include <hacks/features.hh>
#include "renderer.hh"
#include "controls.hh"
#include <hacks/vars.hh>
#include <filesystem>
#include <interfaces/interfaces.hh>

ImVec2 window_menu_size = ImVec2( 625.0f, 50.0f );
ImVec2 window_menu_main = ImVec2( 625.0f, 452.0f );
ImVec2 window_menu_pos = ImVec2( 400.0f, 400.0f );
ImVec2 window_header_position;

bool Mui::Keys[ 256 ] = { 0 };
bool Mui::KeysClicked[ 256 ] = { 0 };
bool Mui::KeysUp[ 256 ] = { 0 };

#define MUI_SIZE( x ) sizeof( x ) / sizeof( *x )

enum MUI_TABS : int {
    MUI_Movement = 0,
    MUI_Visuals,
    MUI_Other,
    MUI_Playerlist,
    MUI_Config
};

const char* autostrafe_modes[ 2 ] = { "telehop", "velocity" };
const char* speedhack_modes[ 6 ] = { "fast", "super fast", "slow", "super slow", "factor", "fast & slow" };
const char* antiafk_modes[ 7 ] = { "side", "forward", "both", "wasd", "roaming", "kill", "gibme" };
const char* antiaim_yaw_modes[ 13 ] = { "none", "zero", "backwards", "sideways", "back jitter", "slowspin", "fastspin", "goldenspin", "random", "fake left", "fake right", "switch", "lisp" };
const char* antiaim_pitch_modes[ 10 ] = { "none", "zero", "emotion", "up", "down", "up2", "fakedown", "fakeup", "varie", "lisp" };
const char* fakelag_modes[ 3 ] = { "on key", "on velocity", "in air" };
const char* box_types[ 3 ] = { "min/max z", "static", "dynamic" };
const char* airstuck_modes[ 5 ] = { "freeze", "packet", "time", "freeze & time", "process" };
const char* vote_modes[ 2 ] = { "no", "yes" };
const char* corpse_modes[ 4 ] = { "angles", "spin", "barrel", "barrel+spin" };

void RenderConfig( Mui::c_window* window ) {
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "configuration", Mui::vec2_t{ 180.0f, 340.0f } );
        static bool b_save_settings = false;
        static bool b_load_settings = false;
        static bool b_unbind_key = false;

        groupbox->add( std::make_unique< Mui::c_label_hotkey >( "Menu key", &cfg::get< int >( vars.menu_key ) ) );
        groupbox->add( std::make_unique< Mui::c_separator >( false, 10.0f ) );

        groupbox->add( std::make_unique< Mui::c_button >( "load settings", &b_load_settings ) );
        if ( b_load_settings ) {
            cfg::load_config( ( std::filesystem::current_path( ) / "hack" / "hack.cfg" ).string( ).data( ) );
            Renderer::PushNotification( "Config has been", " loaded", "!", 3000.0f );
            b_load_settings = false;
        };

        groupbox->add( std::make_unique< Mui::c_separator >( false, 10.0f ) );

        groupbox->add( std::make_unique< Mui::c_button >( "save settings", &b_save_settings ) );
        if ( b_save_settings ) {
            cfg::save_config( ( std::filesystem::current_path( ) / "hack" / "hack.cfg" ).string( ).data( ) );
            Renderer::PushNotification( "Config has been", " saved", "!", 3000.0f );
            b_save_settings = false;
        }

        groupbox->add( std::make_unique< Mui::c_separator >( false, 10.0f ) );
        groupbox->add( std::make_unique< Mui::c_button >( "unbind key", &b_unbind_key ) );
        if ( b_unbind_key ) {
            Renderer::PushNotification( "To unbind a key, click the ", "[...]", " bind and hit escape.", 7000.0f );
            b_unbind_key = false;
        };

        window->add( std::move( groupbox ) );
    }
};

void RenderMovement( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "general", Mui::vec2_t{ 180.0f, 340.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "antiafk", &cfg::get< bool >( vars.antiafk ) ) );

        if ( cfg::get< bool >( vars.antiafk ) ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "antiafk mode", &cfg::get< int >( vars.antiafk_mode ), antiafk_modes, MUI_SIZE( antiafk_modes ) ) );
            switch ( cfg::get< int >( vars.antiafk_mode ) ) {
                case options::antiafk_modes::afk_sidemove:
                    groupbox->add( std::make_unique< Mui::c_slider_float >( "side factor", &cfg::get< float >( vars.antiafk_sidemove ), 1.0f, 370.0f ) );
                    break;
                case options::antiafk_modes::afk_forwardmove:
                    groupbox->add( std::make_unique< Mui::c_slider_float >( "forward factor", &cfg::get< float >( vars.antiafk_forwardmove ), 1.0f, 370.0f ) );
                    break;
                case options::antiafk_modes::afk_both: {
                    groupbox->add( std::make_unique< Mui::c_slider_float >( "side factor", &cfg::get< float >( vars.antiafk_sidemove ), 1.0f, 370.0f ) );
                    groupbox->add( std::make_unique< Mui::c_slider_float >( "forward factor", &cfg::get< float >( vars.antiafk_forwardmove ), 1.0f, 370.0f ) );

                }break;
            };
        }

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "autogauss", &cfg::get< bool >( vars.autogauss ), &cfg::get< int >( vars.autogauss_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "slowwalk", &cfg::get< bool >( vars.slowwalk ), &cfg::get< int >( vars.slowwalk_key ) ) );
        if ( cfg::get< bool >( vars.slowwalk ) )
            groupbox->add( std::make_unique< Mui::c_slider_float >( "slowwalk speed", &cfg::get< float >( vars.slowwalk_speed ), 1.0f, 100.0f ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "bunnyhop", &cfg::get< bool >( vars.bunnyhop ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "autostrafe", &cfg::get< bool >( vars.autostrafe ), &cfg::get< int >( vars.autostrafe_key ) ) );

        if ( cfg::get< bool >( vars.autostrafe ) )
            groupbox->add( std::make_unique< Mui::c_combobox >( "autostrafe mode", &cfg::get< int >( vars.autostrafe_mode ), autostrafe_modes, MUI_SIZE( autostrafe_modes ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "previous weapon", &cfg::get< bool >( vars.fastswitch ), &cfg::get< int >( vars.fastswitch_key ) ) );

        if( cfg::get< int >( vars.speedhack_mode ) == options::speedhack_modes::speed_fast_slow )
            groupbox->add( std::make_unique< Mui::c_checkbox >( "speedhack", &cfg::get< bool >( vars.speedhack ) ) );
        else
            groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "speedhack", &cfg::get< bool >( vars.speedhack ), &cfg::get< int >( vars.speedhack_key ) ) );

        if ( cfg::get< bool >( vars.speedhack ) ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "speedhack mode", &cfg::get< int >( vars.speedhack_mode ), speedhack_modes, MUI_SIZE( speedhack_modes ) ) );

            if ( cfg::get< int >( vars.speedhack_mode ) == options::speedhack_modes::speed_factor ) {
                groupbox->add( std::make_unique< Mui::c_slider_float >( "ups", &cfg::get< float >( vars.speedhack_factor ), 0.0f, 10.0f, "ups" ) );
                groupbox->add( std::make_unique< Mui::c_slider_float >( "multiplier", &cfg::get< float >( vars.speedhack_multiplier ), 0.0f, 50.0f, "x" ) );
            }
            else if ( cfg::get< int >( vars.speedhack_mode ) == options::speedhack_modes::speed_fast_slow ) {
                groupbox->add( std::make_unique< Mui::c_label_hotkey >( "fast key", &cfg::get< int >( vars.speedhack_fast_key ) ) );
                groupbox->add( std::make_unique< Mui::c_label_hotkey >( "slow key", &cfg::get< int >( vars.speedhack_slow_key ) ) );
            };
        };

        groupbox->add( std::make_unique< Mui::c_checkbox >( "jumpbug", &cfg::get< bool >( vars.jumpbug ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "corpse", &cfg::get< bool >( vars.corpse ) ) );

        if ( cfg::get< bool >( vars.corpse ) )
        {
            groupbox->add( std::make_unique< Mui::c_combobox >( "corpse mode", &cfg::get< int >( vars.corpse_mode ), corpse_modes, MUI_SIZE( corpse_modes ) ) );

            switch ( cfg::get< int >( vars.corpse_mode ) ) {
            case 0: {
                groupbox->add( std::make_unique< Mui::c_slider_float >( "corpse x", &cfg::get< float >( vars.corpse_angle_x ), 0.0f, 360.0f ) );
                groupbox->add( std::make_unique< Mui::c_slider_float >( "corpse z", &cfg::get< float >( vars.corpse_angle_z ), 0.0f, 360.0f ) );
            }break;
            case 1:
            case 3: { groupbox->add( std::make_unique< Mui::c_slider_float >( "corpse factor", &cfg::get< float >( vars.corpse_spin ), 0.0f, 5000.0f ) ); } break;
            case 2: 
                groupbox->add( std::make_unique< Mui::c_slider_float >( "corpse barrel", &cfg::get< float >( vars.corpse_spin_barrel ), 0.0f, 5000.0f ) );
            break;
            };
        }

        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "anti-aim", Mui::vec2_t{ 180.0f, 137.0f }, Mui::vec2_t{ 185.0f, 0.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &cfg::get< bool >( vars.antiaim ) ) );

        if ( cfg::get< bool >( vars.antiaim ) ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "yaw", &cfg::get< int >( vars.antiaim_yaw ), antiaim_yaw_modes, MUI_SIZE( antiaim_yaw_modes ) ) );

            groupbox->add( std::make_unique< Mui::c_combobox >( "pitch", &cfg::get< int >( vars.antiaim_pitch ), antiaim_pitch_modes, MUI_SIZE( antiaim_pitch_modes ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "clamp", &cfg::get< bool >( vars.antiaim_clamp ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "3rd person clamp", &cfg::get< bool >( vars.antiaim_clamp_tp ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "simulate view", &cfg::get< bool >( vars.antiaim_simulate ) ) );
        };

        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "fakelag", Mui::vec2_t{ 180.0f, 130.0f }, Mui::vec2_t{ 185.0f, 142.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &cfg::get< bool >( vars.fakelag ) ) );

        if ( cfg::get< bool >( vars.fakelag ) ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "fakelag modes##fakelag", &cfg::get< int >( vars.fakelag_mode ), fakelag_modes, MUI_SIZE( fakelag_modes ) ) );
       //
            if ( cfg::get< int >( vars.fakelag_mode ) == options::fakelag_key ) {
                groupbox->add( std::make_unique< Mui::c_label_hotkey >( "key", &cfg::get< int >( vars.fakelag_key ) ) );
                groupbox->add( std::make_unique< Mui::c_slider_int >( "factor", &cfg::get< int >( vars.fakelag_factor ), 1, 200, "ts" ) );
                groupbox->add( std::make_unique< Mui::c_checkbox >( "toggle", &cfg::get< bool >( vars.fakelag_toggle ) ) );
            };
       };

        window->add( std::move( groupbox ) );
    }
}

void RenderVisuals( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "players", Mui::vec2_t{ 180.0f, 340.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &cfg::get< bool >( vars.visuals ) ) );

        if ( cfg::get< bool >( vars.visuals ) ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "names", &cfg::get< bool >( vars.visuals_name ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "box", &cfg::get< bool >( vars.visuals_box ) ) );

            if ( cfg::get< bool >( vars.visuals_box ) )
                groupbox->add( std::make_unique< Mui::c_color_picker >( "box color", &cfg::get< float >( vars.visuals_hue ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "healthbar", &cfg::get< bool >( vars.visuals_health ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "skin info", &cfg::get< bool >( vars.visuals_skininfo ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "coords", &cfg::get< bool >( vars.visuals_origin ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "angles", &cfg::get< bool >( vars.visuals_angles ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "mins/maxs", &cfg::get< bool >( vars.visuals_minsmaxs ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "snaplines", &cfg::get< bool >( vars.visuals_snaplines ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "dynamic light", &cfg::get< bool >( vars.visuals_dlight ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "only important", &cfg::get< bool >( vars.visuals_only_important ) ) );

            if ( cfg::get< bool >( vars.visuals_dlight ) ) {
                groupbox->add( std::make_unique< Mui::c_color_picker >( "light color", &cfg::get< float >( vars.visuals_dlight_hue ) ) );

                groupbox->add( std::make_unique< Mui::c_slider_float >( "dynamic height", &cfg::get< float >( vars.visuals_dlight_height ), 0.1f, 100.0f ) );
            }

            groupbox->add( std::make_unique< Mui::c_checkbox >( "self", &cfg::get< bool >( vars.visuals_self ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "brightness", &cfg::get< bool >( vars.world_brightness ) ) );

            if ( cfg::get< bool >( vars.world_brightness ) ) {
                groupbox->add( std::make_unique< Mui::c_slider_float >( "brightness level", &cfg::get< float >( vars.world_brightness_factor ), 0.0f, 1.0f ) );
            };

            groupbox->add( std::make_unique< Mui::c_checkbox >( "corpses", &cfg::get< bool >( vars.visuals_corpse ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "dormancy", &cfg::get< bool >( vars.visuals_dormancy ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "debug", &cfg::get< bool >( vars.visuals_debug ) ) );
        }

        window->add( std::move( groupbox ) );
    }
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "mobs", Mui::vec2_t{ 180.0f, 222.0f }, Mui::vec2_t{ 185.0f, 0.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &cfg::get< bool >( vars.mobs ) ) );

        if ( cfg::get< bool >( vars.mobs ) ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "names", &cfg::get< bool >( vars.mobs_name ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "box", &cfg::get< bool >( vars.mobs_box ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "dynamic light", &cfg::get< bool >( vars.mobs_dlight ) ) );

            if ( cfg::get< bool >( vars.mobs_dlight ) ) {
                groupbox->add( std::make_unique< Mui::c_color_picker >( "light color##mobs", &cfg::get< float >( vars.mobs_dlight_hue ) ) );

                groupbox->add( std::make_unique< Mui::c_slider_float >( "dynamic height##mobs", &cfg::get< float >( vars.mobs_dlight_height ), 0.1f, 100.0f ) );
            };
        };

        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "world", Mui::vec2_t{ 180.0f, 112.0f }, Mui::vec2_t{ 185.0f, 228.0f } );
        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &cfg::get< bool >( vars.world ) ) );
        if ( cfg::get< bool >( vars.world ) ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "ammo", &cfg::get< bool >( vars.world_ammo ) ) );
            groupbox->add( std::make_unique< Mui::c_checkbox >( "weapons", &cfg::get< bool >( vars.world_weapons ) ) );
            groupbox->add( std::make_unique< Mui::c_checkbox >( "respawn & boosts", &cfg::get< bool >( vars.world_misc ) ) );
        };
        window->add( std::move( groupbox ) );
    }
}

void RenderPlayerGroup( Mui::c_groupbox* group ) {
    for ( int i = 0; i < xti::g_engine->GetMaxClients( ); i++ ) {
        auto entity = xti::g_engine->GetEntityByIndex( i );
        if ( !entity )
            continue;

        if ( !entity->player || !entity->model )
            continue;

        if ( !entity->model->name )
            continue;

        if ( auto player = xti::g_studiomodel->PlayerInfo( entity->index - 1 ) ) {
            if ( player->name && strlen( player->name ) >= 1 ) {
                group->add( std::make_unique< Mui::c_label >( player->name ) );
            };
        };
    };
};

bool m_player_buttons[ 64 ] = { false };
bool m_player_skins[ 64 ] = { false };
int m_iplayerindex = -1;

void PlayerList( Mui::c_groupbox* grp ) {
    for ( auto it = 0; it != xti::g_engine->GetMaxClients( ); it++ ) {
        auto ent = xti::g_engine->GetEntityByIndex( it );
        if ( !ent )
            continue;

        auto player_info = xti::g_studiomodel->PlayerInfo( ent->index - 1 );
        if ( !player_info || !player_info->name || strlen( player_info->name ) < 1 )
            continue;

        grp->add( std::make_unique< Mui::c_button >( player_info->name, &m_player_buttons[ ent->index ] ) );

        if ( m_player_buttons[ ent->index ] ) {
            m_iplayerindex = ent->index;
            m_player_buttons[ ent->index ] = false;
        };
    };
};

void PlayerFromList( Mui::c_groupbox* grp ) {
    if ( m_iplayerindex == -1 )
        return;

    auto e = xti::g_engine->GetEntityByIndex( m_iplayerindex );
    if ( !e ) {
        m_iplayerindex = -1;
        return;
    };

    auto player_info = xti::g_studiomodel->PlayerInfo( e->index - 1 );
    if ( !player_info || !player_info->name || strlen( player_info->name ) < 1 ) {
        m_iplayerindex = -1;
        return;
    };

    grp->add( std::make_unique< Mui::c_label >( player_info->name ) );
    grp->add( std::make_unique< Mui::c_separator >( ) );
    grp->add( std::make_unique< Mui::c_label >( std::format( "model: {:s}", player_info->model ).data( ) ) );
    grp->add( std::make_unique< Mui::c_label >( std::format( "steamid: {}", player_info->m_nSteamID ).data( ) ) );

    grp->add( std::make_unique< Mui::c_button >( "steal skin", &m_player_skins[ m_iplayerindex ] ) );
    grp->add( std::make_unique< Mui::c_checkbox >( "glue to player", &options::player_glue[ m_iplayerindex ] ) );
    grp->add( std::make_unique< Mui::c_checkbox >( "player important", &options::player_important[ m_iplayerindex ] ) );

    if ( options::player_important[ m_iplayerindex ] ) {
        grp->add( std::make_unique< Mui::c_color_picker >( "player important hue", &options::player_important_hue[ m_iplayerindex ] ) );
        grp->add( std::make_unique< Mui::c_checkbox >( "glow player", &options::player_glow[ m_iplayerindex ] ) );
    };

    if ( m_player_skins[ m_iplayerindex ] ) {
        xti::g_engine->pfnCvar_Set( "model", player_info->model );
        xti::g_engine->pfnCvar_Set( "topcolor", std::format( "{}", player_info->topcolor ).data( ) );
        xti::g_engine->pfnCvar_Set( "bottomcolor", std::format( "{}", player_info->bottomcolor ).data( ) );
        Renderer::PushNotification( "Snatched ", player_info->name, " player skin, this belongs to you now." );
        m_player_skins[ m_iplayerindex ] = false;
    };
};

void RenderPlayerList( Mui::c_window* window ) {
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "players", Mui::vec2_t{ 180.0f, 250.0f } );
        if ( !xti::g_rungameengine->is_in_game( ) )
            groupbox->add( std::make_unique< Mui::c_label >( "you're not in a server." ) );
        else {
            PlayerList( groupbox.get( ) );
        };
        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "player actions", Mui::vec2_t{ 180.0f, 250.0f }, Mui::vec2_t{ 185.0f, 0.0f } );
        if ( xti::g_rungameengine->is_in_game( ) )
            PlayerFromList( groupbox.get( ) );
        window->add( std::move( groupbox ) );
        //auto groupbox = std::make_unique< Mui::c_groupbox >( "players", Mui::vec2_t{ 180.0f, 250.0f } );
    }
};

bool set_changed_model = false;

void RenderOther( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "other #1", Mui::vec2_t{ 180.0f, 250.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "movement recorder", &cfg::get< bool >( vars.movement ) ) );

        if ( cfg::get< bool >( vars.movement ) ) {
            groupbox->add( std::make_unique< Mui::c_label_hotkey >( "record movement", &cfg::get< int >( vars.movement_record ) ) );

            groupbox->add( std::make_unique< Mui::c_label_hotkey >( "playback movement", &cfg::get< int >( vars.movement_play ) ) );

            groupbox->add( std::make_unique< Mui::c_button >( "clear movement", &cfg::get< bool >( vars.movement_clear ) ) );

        };

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "self sink", &cfg::get< bool >( vars.selfsink ), &cfg::get< int >( vars.selfsink_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "spam use", &cfg::get< bool >( vars.usespam ), &cfg::get< int >( vars.usespam_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "spam flash", &cfg::get< bool >( vars.flashspam ), &cfg::get< int >( vars.flashspam_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "auto crossbow boost", &cfg::get< bool >( vars.autocross ), &cfg::get< int >( vars.autocross_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "airstuck", &cfg::get< bool >( vars.airstuck ), &cfg::get< int >( vars.airstuck_key ) ) );

        if ( cfg::get< bool >( vars.airstuck ) ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "airstuck modes", &cfg::get< int >( vars.airstuck_mode ), airstuck_modes, MUI_SIZE( airstuck_modes ) ) );
            
            groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "airstuck [dead]", &cfg::get< bool >( vars.airstuck_on_death ), &cfg::get< int >( vars.airstuck_on_death_key ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "anti-afk - airstuck air", &cfg::get< bool >( vars.airstuck_air_afk ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "anti-afk - airstuck dead", &cfg::get< bool >( vars.airstuck_dead_afk ) ) );
        };

        groupbox->add( std::make_unique< Mui::c_checkbox >( "vote", &cfg::get< bool >( vars.vote ) ) );
        
        if ( cfg::get< bool >( vars.vote ) ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "[vote] disable", &cfg::get< bool >( vars.vote_disable ) ) );
            groupbox->add( std::make_unique< Mui::c_checkbox >( "[vote] auto", &cfg::get< bool >( vars.vote_auto ) ) );
            if( cfg::get< bool >( vars.vote_auto ) )
                groupbox->add( std::make_unique< Mui::c_combobox >( "[vote] modes", &cfg::get< int >( vars.vote_mode ), vote_modes, MUI_SIZE( vote_modes ) ) );
        };

        groupbox->add( std::make_unique< Mui::c_checkbox >( "scr_update rate", &cfg::get< bool >( vars.scr ) ) );
        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "other #2", Mui::vec2_t{ 180.0f, 272.0f }, Mui::vec2_t{ 185.0f, 0.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "follow player", &cfg::get< bool >( vars.follow_player ) ) );

        if ( cfg::get< bool >( vars.follow_player ) ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "follow important", &cfg::get< bool >( vars.follow_important ) ) );
            groupbox->add( std::make_unique< Mui::c_slider_float >( "follow dist", &cfg::get< float >( vars.follow_distance ), 1.0f, 100.0f ) );
            groupbox->add( std::make_unique< Mui::c_checkbox >( "steal player skin", &cfg::get< bool >( vars.follow_steal_skin ) ) );
        }

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "mirror cam", &cfg::get< bool >( vars.mirror ), &cfg::get< int >( vars.mirror_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "air run (cancel anim)", &cfg::get< bool >( vars.airrun ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "fastrun", &cfg::get< bool >( vars.fastrun ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "chat spam", &cfg::get< bool >( vars.chatspam ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "tickbase manipulation", &cfg::get< bool >( vars.rev_expl ), &cfg::get< int >( vars.rev_expl_key ) ) );
        if ( cfg::get< bool >( vars.rev_expl ) ) {
            groupbox->add( std::make_unique< Mui::c_slider_int >( "exploit factor", &cfg::get< int >( vars.rev_expl_factor ), 0, 70 ) );
            groupbox->add( std::make_unique< Mui::c_slider_float >( "spms", &cfg::get< float >( vars.rev_expl_factor2 ), 0.0f, 1000.0f ) );
        };

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "dupe glitch", &cfg::get< bool >( vars.dupeglitch ), &cfg::get< int >( vars.dupeglitch_key ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "crosshair", &cfg::var( vars.crosshair ).as< bool >( ) ) );

        if ( cfg::get< bool >( vars.crosshair ) ) {
            static float m_sat = 1.0f;
            static float m_val = 1.0f;
            groupbox->add( std::make_unique< Mui::c_color_picker >( "crosshair hue", &cfg::get< float >( vars.crosshair_hue ), cfg::get< float >( vars.crosshair_sat ) ) );
            groupbox->add( std::make_unique< Mui::c_saturation_slider >( "crosshair sat", &cfg::get< float >( vars.crosshair_sat ) ) );
            groupbox->add( std::make_unique< Mui::c_slider_float >( "crosshair gap", &cfg::get< float >( vars.crosshair_gap ), 0.0f, 100.0f ) );
        }

        groupbox->add( std::make_unique< Mui::c_checkbox >( "draw push notifications", &cfg::var( vars.notifications ).as< bool >( ) ) );
        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "other #3", Mui::vec2_t{ 180.0f, 100.0f }, Mui::vec2_t{ 0.0f, 254.0f } );
        groupbox->add( std::make_unique< Mui::c_color_picker >( "model color [top]", &cfg::get< float >( vars.model_color_top ) ) );
        groupbox->add( std::make_unique< Mui::c_color_picker >( "model color [bottom]", &cfg::get< float >( vars.model_color_bottom ) ) );
        groupbox->add( std::make_unique< Mui::c_button >( "update model color", &set_changed_model ) );
        groupbox->add( std::make_unique< Mui::c_checkbox >( "model pulsator", &cfg::get< bool >( vars.model_pulsator ) ) );
        if ( cfg::get< bool >( vars.model_pulsator ) ) {
            groupbox->add( std::make_unique< Mui::c_slider_float >( "pulsator delay", &cfg::get< float >( vars.model_pulsator_delay ), 1.0f, 1500.0f, "ms" ) );
            groupbox->add( std::make_unique< Mui::c_checkbox >( "inverse bottom pulsator", &cfg::get< bool >( vars.model_inverse_pulastor ) ) );
        };

        if ( set_changed_model ) {
            Renderer::PushNotification( "Model", " color ", "has been updated." );
            xti::g_engine->pfnCvar_Set( "topcolor", std::format( "{}", cfg::get< float >( vars.model_color_top ) * 255.0f ).data( ) );
            xti::g_engine->pfnCvar_Set( "bottomcolor", std::format( "{}", cfg::get< float >( vars.model_color_bottom ) * 255.0f ).data( ) );
            set_changed_model = false;
        };
        window->add( std::move( groupbox ) );
    };
};

static bool is_holding_menu = false;
void Mui::RenderOldMenu( ) {
    Renderer::Setup( ImGui::GetBackgroundDrawList( ) );

    if ( options::show_menu ) {
        float posx = cfg::get< float >( vars.menu_pos_x );
        float posy = cfg::get< float >( vars.menu_pos_y );

        auto window = std::make_unique< Mui::c_window >( "hack", Mui::vec2_t( posx, posy ), _CORE_BUILD_DATE );
        window->begin( );
        window->tabs< const char*, const char*, const char*, const char*, const char* >( &cfg::get< int >( vars.menu_index ), "move", "visuals", "misc", "playerlist", "cfg" );
        switch ( static_cast< MUI_TABS >( cfg::get< int >( vars.menu_index ) ) ) {
        case MUI_Movement:
            RenderMovement( window.get( ) );
            break;
        case MUI_Visuals:
            RenderVisuals( window.get( ) );
            break;
        case MUI_Other:
            RenderOther( window.get( ) );
            break;

        case MUI_Playerlist:
            RenderPlayerList( window.get( ) );
            break;

        case MUI_Config:
            RenderConfig( window.get( ) );
            break;
        default:
            break;
        };
        window->end( );
        if ( io.MousePos.x > posx && io.MousePos.y > posy && io.MousePos.x < posx + 50.0f && io.MousePos.y < posy + 20.0f )
            if ( io.MouseDown[ 0 ] )
                is_holding_menu = true;

        if ( !io.MouseDown[ 0 ] && is_holding_menu )
            is_holding_menu = false;

        if ( is_holding_menu ) {
            cfg::get< float >( vars.menu_pos_x ) = io.MousePos.x;
            cfg::get< float >( vars.menu_pos_y ) = io.MousePos.y;
        }
    };
};

void Mui::RenderNewMenu( ) { 

};

void Mui::Render( ) {
    RenderOldMenu( );
};

void Mui::Setup( ) {
    auto& io = ImGui::GetIO( );
    io.IniFilename = NULL;

    auto& style = ImGui::GetStyle( );
    style.AntiAliasedLines = false;
    style.AntiAliasedFill = false;

    ImFontConfig cfg;
    Renderer::g_pFont = io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf", 10.0f, NULL, ImGui::GetIO( ).Fonts->GetGlyphRangesDefault( ) );
    Renderer::g_pGilroy = io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Verdana.ttf", 12.0f, NULL, ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );
}
void Mui::set_key_down( int k )
{
    Mui::Keys[ k ] = true;
};

void Mui::set_key_toggle( int k )
{
    Mui::KeysUp[ k ] = !Mui::KeysUp[ k ];
};

void Mui::reset_key_down( int k )
{
    Mui::Keys[ k ] = false;
};

void Mui::reset_key_toggle( int k )
{
    Mui::KeysUp[ k ] = false;
}
bool Mui::is_menu_present( )
{
    return options::show_menu;
};

bool Mui::is_key_toggle( int k )
{
    if ( k == static_cast< int >( 0x1b ) )
        return false;

    return Mui::KeysUp[ k ];
};

bool Mui::is_key_down( int k )
{
    if ( k == static_cast< int >( 0x1b ) )
        return false;

    return Mui::Keys[ k ];
};
#include "menu.hh"
#include <imgui/imgui.h>
#include <hacks/features.hh>
#include "renderer.hh"
#include "controls.hh"

#include <interfaces/interfaces.hh>

ImVec2 window_menu_size = ImVec2( 625.0f, 50.0f );
ImVec2 window_menu_main = ImVec2( 625.0f, 372.0f );
ImVec2 window_header_position;

bool Mui::Keys[ 256 ] = { 0 };
bool Mui::KeysClicked[ 256 ] = { 0 };

#define MUI_SIZE( x ) sizeof( x ) / sizeof( *x )

enum MUI_TABS : int {
    MUI_Movement = 0,
    MUI_Visuals,
    MUI_Other,
    MUI_Config
};

const char* autostrafe_modes[ 2 ] = { "telehop", "velocity" };
const char* speedhack_modes[ 4 ] = { "fast", "super fast", "slow", "super slow" };
const char* antiafk_modes[ 5 ] = { "forward", "side", "both", "roaming", "walkbot" };
const char* antiaim_yaw_modes[ 9 ] = { "backwards", "sideways", "back jitter", "slowspin", "fastspin", "random", "fake left", "fake right", "switch" };
const char* antiaim_pitch_modes[ 6 ] = { "emotion", "up", "down", "fakedown", "270.0f", "lisp" };
const char* fakelag_modes[ 2 ] = { "on key", "on velocity" };
const char* box_types[ 3 ] = { "min/max z", "static", "dynamic" };

void RenderVisuals( Mui::vec2_t pos, Mui::vec2_t size ) {
    Renderer::AddRect( pos, size, ImColor( 0, 0, 0, 150 ) );

    Mui::vec2_t menu_relative_position = pos + 1.0f;
    Mui::vec2_t menu_relative_size = size - 2.0f;

    Renderer::AddRectFilled( menu_relative_position, menu_relative_size, ImColor( 25, 25, 25, 255 ) ); // Background
    Renderer::AddRectFilled( menu_relative_position, ImVec2( menu_relative_size.x, 20.0f ), ImColor( 23, 25, 28, 255 ) ); // Header
    Renderer::AddRectFilled( ImVec2( menu_relative_position.x, menu_relative_position.y + 20.0f ), ImVec2( menu_relative_size.x, 2.0f ), menucolor ); // Bar

    Renderer::AddText( ImVec2( menu_relative_position.x + 5.0f/*menu_relative_size.x - title_position.x - 10.0f*/, menu_relative_position.y + 5.0f ), ImColor( 255, 255, 255, 255 ), "visuals - preview" );

    Mui::vec2_t box_region = { pos.x + 35.0f, pos.y + 50.0f };
    Mui::vec2_t box_size = { size.x - 55.0f, size.y - 70.0f };

    if ( options::visuals::box )
        Renderer::AddRectOutline( box_region, box_size, ImColor( 255, 25, 25, 255 ) );

    if ( options::visuals::names )
        Renderer::AddText( { box_region.x + box_size.x / 2.0f, box_region.y - 10.0f }, ImColor( 255, 255, 255, 255 ), "guwi", true );

    if ( options::visuals::weapons )
        Renderer::AddText( { box_region.x + box_size.x / 2.0f, box_region.y + box_size.y + 5.0f }, ImColor( 255, 255, 255, 255 ), "gauss", true );

    if ( options::visuals::healthbar ) {
        Renderer::AddRectFilled( { box_region.x - 4.0f, box_region.y }, { 3.0f, box_size.y }, ImColor( 25, 25, 25, 180 ) );
        Renderer::AddRectFilled( { box_region.x - 4.0f, box_region.y }, { 1.0f, box_size.y }, ImColor( 255, 255, 25, 180 ) );
    };
};

void RenderMovement( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "general", Mui::vec2_t{ 180.0f, 222.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "airstuck", &options::general::airstuck, &options::general::airstuck_key ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "antiafk", &options::general::antiafk ) );

        if ( options::general::antiafk )
            groupbox->add( std::make_unique< Mui::c_combobox >( "antiafk mode", &options::general::antiafk_mode, antiafk_modes, MUI_SIZE( antiafk_modes ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "autogauss", &options::general::autogauss, &options::general::autogauss_key ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "slowwalk", &options::general::slowwalk, &options::general::slowwalk_key ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "bunnyhop", &options::general::bhop ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "autostrafe", &options::general::autostrafe, &options::general::autostrafe_key ) );

        if ( options::general::autostrafe )
            groupbox->add( std::make_unique< Mui::c_combobox >( "autostrafe mode", &options::general::autostrafe_mode, autostrafe_modes, MUI_SIZE( autostrafe_modes ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "previous weapon", &options::general::last_inventory, &options::general::last_inventory_key ) );

        groupbox->add( std::make_unique< Mui::c_checkbox_hotkey >( "speedhack", &options::general::speedhack, &options::general::speedhack_key ) );

        if ( options::general::speedhack )
            groupbox->add( std::make_unique< Mui::c_combobox >( "speedhack mode", &options::general::speedhack_mode, speedhack_modes, MUI_SIZE( speedhack_modes ) ) );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "jumpbug", &options::general::jumpbug ) );

        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "anti-aim", Mui::vec2_t{ 180.0f, 137.0f }, Mui::vec2_t{ 185.0f, 0.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &options::antiaim::enabled ) );

        if ( options::antiaim::enabled ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "yaw", &options::antiaim::antiaim_yaw, antiaim_yaw_modes, MUI_SIZE( antiaim_yaw_modes ) ) );

            groupbox->add( std::make_unique< Mui::c_combobox >( "pitch", &options::antiaim::antiaim_pitch, antiaim_pitch_modes, MUI_SIZE( antiaim_pitch_modes ) ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "clamp", &options::antiaim::clamp ) );
        };

        window->add( std::move( groupbox ) );
    }

    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "fakelag", Mui::vec2_t{ 180.0f, 80.0f }, Mui::vec2_t{ 185.0f, 142.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &options::fakelag::enabled ) );

        if ( options::fakelag::enabled ) {
            groupbox->add( std::make_unique< Mui::c_combobox >( "fakelag modes", &options::fakelag::fakelag_mode, fakelag_modes, MUI_SIZE( fakelag_modes ) ) );

            if ( options::fakelag::fakelag_mode == 0 ) {
                groupbox->add( std::make_unique< Mui::c_label_hotkey >( "fakelag key", &options::fakelag::fakelag_key ) );
            };
        };

        window->add( std::move( groupbox ) );
    }
}

void RenderVisuals( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "players", Mui::vec2_t{ 180.0f, 222.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &options::visuals::enabled ) );

        if ( options::visuals::enabled ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "names", &options::visuals::names ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "box", &options::visuals::box ) );

            //if ( options::visuals::box ) {
            //    groupbox->add( std::make_unique< Mui::c_combobox >( "box mode", &options::visuals::box_mode, box_types, MUI_SIZE( box_types ) ) );
            //};

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "weapons", &options::visuals::weapons ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "healthbar", &options::visuals::healthbar ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "skeleton", &options::visuals::skeleton ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "chams", &options::visuals::chams ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "xqz", &options::visuals::xqz ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "glow", &options::visuals::glow ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "dlight", &options::visuals::dlight ) );
       
            groupbox->add( std::make_unique< Mui::c_checkbox >( "brightness", &options::visuals::brightness ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "debug", &options::visuals::debug ) );
        }

        window->add( std::move( groupbox ) );
    }
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "mobs", Mui::vec2_t{ 180.0f, 222.0f }, Mui::vec2_t{ 185.0f, 0.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "enabled", &options::mobs::enabled ) );

        if ( options::mobs::enabled ) {
            groupbox->add( std::make_unique< Mui::c_checkbox >( "names", &options::mobs::names ) );

            groupbox->add( std::make_unique< Mui::c_checkbox >( "box", &options::mobs::box ) );

            //if ( options::mobs::box ) {
            //    groupbox->add( std::make_unique< Mui::c_combobox >( "box mode#mobs#", &options::mobs::box_mode, box_types, MUI_SIZE( box_types ) ) );
            //};

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "skeleton", &options::mobs::skeleton ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "chams", &options::mobs::chams ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "xqz", &options::mobs::xqz ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "glow", &options::mobs::glow ) );

            //groupbox->add( std::make_unique< Mui::c_checkbox >( "dlight", &options::mobs::dlight ) );
        }

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

void RenderOther( Mui::c_window* window ) {
    // Groupbox #1
    {
        auto groupbox = std::make_unique< Mui::c_groupbox >( "other", Mui::vec2_t{ 180.0f, 222.0f } );

        groupbox->add( std::make_unique< Mui::c_checkbox >( "movement recorder", &options::recorder::enabled ) );

        if ( options::recorder::enabled ) {
            groupbox->add( std::make_unique< Mui::c_label_hotkey >( "record movement", &options::recorder::record_record ) );

            groupbox->add( std::make_unique< Mui::c_label_hotkey >( "playback movement", &options::recorder::record_play ) );

            groupbox->add( std::make_unique< Mui::c_button >( "clear movement", &options::recorder::record_clear ) );
        };

        window->add( std::move( groupbox ) );
    }
    //{
    //    auto groupbox = std::make_unique< Mui::c_groupbox >( "info", Mui::vec2_t{ 180.0f, 222.0f }, Mui::vec2_t{ 185.0f, 0.0f } );
    //
    //    window->add( std::move( groupbox ) );
    //}
}

void Mui::Render( ) {
    Renderer::Setup( ImGui::GetBackgroundDrawList( ) );
    if ( options::show_menu ) {
        auto window = std::make_unique< Mui::c_window >( "mui framework", Mui::vec2_t( 400.0f, 400.0f ) );
        window->begin( );
        {
            window->tabs( &options::show_menu_tab, "movement", "visuals", "other" );

            switch ( static_cast< MUI_TABS >( options::show_menu_tab ) ) {
            case MUI_Movement:
                RenderMovement( window.get( ) );
                break;
            case MUI_Visuals:
                RenderVisuals( window.get( ) );
                break;
            case MUI_Other:
                RenderOther( window.get( ) );
                break;
                //case MUI_Config:
                //    RenderMovement( window.get( ) );
                //    break;
            default:
                break;
            };
        }
        vec2_t menusize = window->get_size( );
        vec2_t menupos = window->get_position( );
        window->end( );

        if ( options::visuals::enabled && static_cast< MUI_TABS >( options::show_menu_tab ) == MUI_Visuals )
            RenderVisuals( { menupos.x + menusize.x + 5.0f, menupos.y }, { 180.0f, menusize.y } );
    };
};

void Mui::Setup( ) {
    auto& io = ImGui::GetIO( );
    ImFontConfig cfg;
    Renderer::g_pFont = io.Fonts->AddFontFromFileTTF( "C:\\Users\\guwi\\AppData\\Local\\Microsoft\\Windows\\Fonts\\04B_03_.TTF", 8.0f, NULL, ImGui::GetIO( ).Fonts->GetGlyphRangesCyrillic( ) );
};
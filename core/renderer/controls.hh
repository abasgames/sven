#pragma once
#include <windows.h>
#include <imgui/imgui.h>
#include <format>
#include <memory>
#include <array>
#include <string>
#include <unordered_map>

#define menucolor ImColor( 50, 135, 200, 255 )
#define MUI_BUILD "Build date: " __DATE__ " @ " __TIME__

namespace Mui {
    char username_buffer[ 0xff ];
    const char* GetPCName( ) {
        auto username_length = static_cast< DWORD >( 0xff );
        GetUserNameA( username_buffer, &username_length );
        return username_buffer;
    };
    struct vec2_t {
        float x, y;
        vec2_t( ) : x( 0.0f ), y( 0.0f ) { };
        vec2_t( float xy ) : x( xy ), y( xy ) { };
        vec2_t( float _x, float _y ) : x( _x ), y( _y ) { };

        inline vec2_t operator+( const vec2_t& v ) { return { x + v.x, y + v.y }; };

        inline vec2_t operator&( const ImVec2 v ) { return { v.x, v.y }; }

        inline vec2_t operator+( const float v ) { return { x + v, y + v }; };

        inline vec2_t operator-( const vec2_t& v ) { return { x - v.x, y - v.y }; };

        inline vec2_t operator-( const float v ) { return { x - v, y - v }; };

        inline vec2_t operator/( const vec2_t& v ) { return { x / v.x, y / v.y }; };

        inline vec2_t operator/( const float v ) { return { x / v, y / v }; };

        inline vec2_t operator+=( const vec2_t& v ) { return { x += v.x, y += v.y }; };

        inline ImVec2 to_imvec( ) const noexcept {
            return ImVec2( x, y );
        };

        inline operator ImVec2( ) {
            return ImVec2( x, y );
        };

        inline constexpr bool is_zero( ) const { return x == 0.0f && y == 0.0f; };
    };

    const char* get_key_name( int* key ) {

        switch ( *key ) {
        case VK_LBUTTON:
            return "[M1]";
        case VK_RBUTTON:
            return "[M2]";
        case VK_MBUTTON:
            return "[M3]";
        case VK_XBUTTON1:
            return "[M4]";
        case VK_XBUTTON2:
            return "[M5]";
        case VK_LSHIFT:
        case VK_RSHIFT:
        case VK_SHIFT:
            return "[SHIFT]";
        default:
        {
            UINT scanCode = MapVirtualKeyW( *key, MAPVK_VK_TO_VSC );
            LONG lParamValue = ( scanCode << 16 );
            char key_buffer[ 256 ];
            GetKeyNameTextA( lParamValue, key_buffer, sizeof( key_buffer ) );

            return std::format( "[{:s}]", key_buffer ).c_str( );
        }
        };
    };
    vec2_t menu_position;
    vec2_t menu_size;
    class c_controls {
    public:
        vec2_t position;
        virtual ~c_controls( ) = default;

    public: // virtual child functions
        virtual void on_render( ) = 0;
        virtual void on_post_event( ) = 0;
        virtual void on_pre_event( ) = 0;

    public: // public functions
        void set_position( const vec2_t pos ) { position = pos; };
        vec2_t get_padding( ) const noexcept { return padding; };

        c_controls* get( ) {
            return this;
        };
    protected:
        vec2_t padding;
    };

    struct childcontrols_t {
        bool opened; // comboboxes
        bool hotkey; // hotkeys
        bool holding; // sliders
        c_controls* control;
    };

    c_controls* parent = nullptr;
    std::unordered_map< const char*, childcontrols_t > children;

    // const char* Title, bool* Var
    class c_checkbox : public c_controls {
    public:
        c_checkbox( const char* name, bool* var ) {
            childname = name;
            padding = { 0.0f, 15.0f };
            size = { 12.0f, 12.0f };
            pvar = var;
        };

        void on_render( ) override {
            Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y + 2.0f }, ImColor( 255, 255, 255, 255 ), childname );
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, *pvar ? menucolor : ImColor( 23, 23, 23, 255 ) );
            Renderer::AddRect( { position.x, position.y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        };

        void on_post_event( ) override {
            auto io = ImGui::GetIO( );
            if ( is_hovering( ) )
                if ( io.MouseClicked[ 0 ] && parent == nullptr ) {
                    parent = this;
                }

            if ( parent == this ) {
                *pvar = !*pvar;
                parent = nullptr;
            }
        };

        void on_pre_event( ) override {

        };
    private:
        bool is_hovering( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y && coords.x < position.x + size.x && coords.y < position.y + size.y );
        };
    private:
        const char* childname;
        bool* pvar;
        vec2_t size;
    };

    // const char* Title, int* Key
    class c_label_hotkey : public c_controls {
    public:
        c_label_hotkey( const char* name, int* key ) {
            childname = name;
            padding = { 0.0f, 15.0f };
            size = { 12.0f, 12.0f };
            pkey = key;
        };

        void on_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos ) {
                Renderer::AddTextShadow( { position.x, position.y + 2.0f }, ImColor( 255, 255, 255, 255 ), str_converted.erase( first, second ).c_str( ) );
            }
            else {
                Renderer::AddTextShadow( { position.x, position.y + 2.0f }, ImColor( 255, 255, 255, 255 ), childname );
            }

            //UINT scanCode = MapVirtualKeyW( *pkey, MAPVK_VK_TO_VSC );
            //LONG lParamValue = ( scanCode << 16 );
            //char key_buffer[ 256 ];
            //GetKeyNameTextA( lParamValue, key_buffer, sizeof( key_buffer ) );
            //
            //auto buffer_data = std::format( "[{:s}]", children[ childname ].hotkey ? "..." : key_buffer );
            //const auto to_buffer = buffer_data.data( );
            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, to_buffer );
            Renderer::AddTextShadow( { position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f }, ImColor( 100, 100, 100, 255 ), to_buffer );
        };

        void on_post_event( ) override {
            auto io = ImGui::GetIO( );

            if ( is_hovering_key( ) ) {
                if ( io.MouseClicked[ 0 ] )
                    children[ childname ].hotkey = true;
            }

            if ( children[ childname ].hotkey ) {
                for ( int i = 0; i < 256; i++ ) {
                    if ( Mui::Keys[ i ] ) {
                        *pkey = i;
                        children[ childname ].hotkey = false;
                    }
                }
            }

        };

        void on_pre_event( ) override {

        };
    private:
        bool is_hovering_key( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, to_buffer );
            auto frame_position = vec2_t{ position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f };
            return ( coords.x > frame_position.x && coords.y > frame_position.y && coords.x < frame_position.x + m_text_size.x && coords.y < frame_position.y + m_text_size.y );
        };
    private:
        const char* childname;
        int* pkey;
        vec2_t size;
    };

    // const char* Title, bool* Var, int* Key
    class c_checkbox_hotkey : public c_controls {
    public:
        c_checkbox_hotkey( const char* name, bool* var, int* key ) {
            childname = name;
            padding = { 0.0f, 15.0f };
            size = { 12.0f, 12.0f };
            pvar = var;
            pkey = key;
        };

        void on_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos ) {
                Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y + 2.0f }, ImColor( 255, 255, 255, 255 ), str_converted.erase( first, second ).c_str( ) );
            }
            else {
                Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y + 2.0f }, ImColor( 255, 255, 255, 255 ), childname );
            }
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, *pvar ? menucolor : ImColor( 23, 23, 23, 255 ) );
            Renderer::AddRect( { position.x, position.y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, to_buffer );
            Renderer::AddTextShadow( { position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f }, ImColor( 100, 100, 100, 255 ), to_buffer );
        };

        void on_post_event( ) override {
            auto io = ImGui::GetIO( );
            if ( is_hovering( ) )
                if ( io.MouseClicked[ 0 ] && parent == nullptr ) {
                    parent = this;
                }

            if ( parent == this ) {
                *pvar = !*pvar;
                parent = nullptr;
            }

            if ( is_hovering_key( ) ) {
                if ( io.MouseClicked[ 0 ] )
                    children[ childname ].hotkey = true;
            }

            if ( !children[ childname ].hotkey )
                return;

            for ( int i = 0; i < 256; i++ ) {
                if ( Mui::Keys[ i ] ) {
                    *pkey = i;
                    children[ childname ].hotkey = false;
                }
            }
        };

        void on_pre_event( ) override {

        };
    private:
        bool is_hovering( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y && coords.x < position.x + size.x && coords.y < position.y + size.y );
        };

        bool is_hovering_key( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, to_buffer );
            auto frame_position = vec2_t{ position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f };
            return ( coords.x > frame_position.x && coords.y > frame_position.y && coords.x < frame_position.x + m_text_size.x && coords.y < frame_position.y + m_text_size.y );
        };
    private:
        const char* childname;
        bool* pvar;
        int* pkey;
        vec2_t size;
    };

    // const char* Title, int* Selected, const char* const* Items, int ItemSize
    class c_combobox : public c_controls {
    public:
        c_combobox( const char* item, int* selected, const char* const* items, int items_size ) {
            childname = item;
            size = { 80.0f, 12.0f };
            padding = { 0.0f, 15.0f };
            m_items = items;
            m_items_size = items_size;
            selected_index = selected;

            children[ childname ].control = this;
        };

        void on_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos ) {
                Renderer::AddTextShadow( { position.x + size.x + 5.0f, position.y }, ImColor( 255, 255, 255, 255 ), str_converted.erase( first, second ).c_str( ) );
            }
            else {
                Renderer::AddTextShadow( { position.x + size.x + 5.0f, position.y }, ImColor( 255, 255, 255, 255 ), childname );
            }
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
            Renderer::AddRect( { position.x, position.y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
            Renderer::AddTextShadow( { position.x + 6.0f, position.y + 1.0f }, children[ childname ].opened ? menucolor : ImColor( 255, 255, 255, 255 ), m_items[ *selected_index ] );

            //vec2_t arrow_base_location = position + size - 8.0f;
            //position.x += 5.0f;
            //constexpr float arrow_size = 4.0f;
            //ImVec2 arrow_polys[3] = {
            //    ImVec2( arrow_base_location.x + ( arrow_size / 2.0f ), arrow_base_location.y ),
            //    ImVec2( arrow_base_location.x, arrow_base_location.y + arrow_size ),
            //    ImVec2( arrow_base_location.x + arrow_size, arrow_base_location.y + arrow_size )
            //};
            //Renderer::AddPoly( arrow_polys, 3, ImColor( 255, 255, 255, 255 ) );

            Renderer::AddTextShadow( { position.x + size.x - 6.0f, position.y + 1.0f }, children[ childname ].opened ? menucolor : ImColor( 255, 255, 255, 255 ), children[ childname ].opened ? "-" : "+" );

            // render all the items
            if ( !children[ childname ].opened )
                return;

            for ( int item_next_index = 0; item_next_index < m_items_size; item_next_index++ ) {
                auto current_item = m_items[ item_next_index ];
                if ( !current_item )
                    continue;

                bool is_hover = is_hovering_custom( position.x, position.y + size.y + item_next_index * size.y, size.x, size.y );
                Renderer::AddRectFilled( { position.x, position.y + size.y + item_next_index * size.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
                Renderer::AddTextShadow( { position.x + 6.0f, position.y + size.y + item_next_index * size.y }, is_hover ? menucolor : ImColor( 255, 255, 255, 255 ), current_item );
            };
        };

        void on_post_event( ) override {
            auto io = ImGui::GetIO( );
            if ( parent == nullptr ) {
                if ( is_hovering( ) ) {
                    if ( io.MouseClicked[ 0 ] ) {
                        parent = children[ childname ].control;
                        if ( parent == children[ childname ].control )
                            children[ childname ].opened = !children[ childname ].opened;
                    };
                };
            }


            if ( children[ childname ].opened ) {
                bool is_outside = !is_hovering_custom( position.x, position.y, size.x, size.y + m_items_size * size.y );
                if ( is_outside && io.MouseClicked[ 0 ] ) {
                    children[ childname ].opened = false;
                    parent = nullptr;
                }
            };

            // handle all the logic
            if ( children[ childname ].opened ) {
                for ( int item_next_index = 0; item_next_index < m_items_size; item_next_index++ ) {
                    auto current_item = m_items[ item_next_index ];
                    if ( !current_item )
                        continue;

                    if ( !is_hovering_custom( position.x, position.y + size.y + item_next_index * size.y, size.x, size.y ) )
                        continue;

                    if ( !io.MouseClicked[ 0 ] )
                        continue;

                    *selected_index = item_next_index;
                    children[ childname ].opened = !children[ childname ].opened;
                    parent = nullptr;
                    //Renderer::AddRectFilled( { position.x, position.y + size.y + item_next_index * size.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
                    //Renderer::AddTextShadow( { position.x + 6.0f, position.y + size.y + item_next_index * size.y }, ImColor( 255, 255, 255, 255 ), current_item );
                };
            }

        };

        void on_pre_event( ) override {

        };
    private:
        bool is_hovering( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y && coords.x < position.x + size.x && coords.y < position.y + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };

    private:
        int* selected_index;
        const char* childname;
        vec2_t size;
        const char* const* m_items;
        int m_items_size;
    };

    // const char* Title
    class c_label : public c_controls {
    public:
        c_label( const char* name ) {
            label = name;
            padding = { 0.0f, 10.0f };
        };

        void on_render( ) override {
            Renderer::AddTextShadow( { position.x, position.y }, ImColor( 255, 255, 255, 255 ), label );
        };

        void on_post_event( ) override {

        };

        void on_pre_event( ) override {

        };
    private:
        const char* label;
    };

    // bool Border, float NextPadding
    class c_separator : public c_controls {
    public:
        c_separator( bool border = false, float next_padding = 10.0f ) {
            padding = { 0.0f, next_padding };
            show_border = border;
        };

        void on_render( ) override {
            if ( show_border )
                Renderer::AddLine( { position.x, position.y + ( padding.y / 2.0f ) }, { position.x + 90.0f, position.y + ( padding.y / 2.0f ) }, ImColor( 255, 255, 255, 255 ) );
        };

        void on_post_event( ) override { };

        void on_pre_event( ) override { };
    private:
        bool show_border;
    };

    // const char* Title, bool* Item, vec2_t ButtonSize
    class c_button : public c_controls {
    public:
        c_button( const char* item, bool* btn, vec2_t btnsize = { 0.0f, 0.0f } ) {
            padding = { 0.0f, 10.0f };
            size = btnsize;
            childname = item;
            clicked = btn;

            if ( size.is_zero( ) ) {
                const auto text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, childname );
                constexpr float selfpad = 6.0f;
                size = { text_size.x + selfpad, text_size.y + selfpad };
                padding.y += selfpad;
            }
            else {
                padding.y += size.y;
            };
        };

        void on_render( ) override {
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
            Renderer::AddTextShadow( position + size / 2.0f, is_hovering( ) ? menucolor : ImColor( 255, 255, 255, 255 ), childname, true );
            Renderer::AddRect( { position.x, position.y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        };

        void on_post_event( ) override {
            auto io = ImGui::GetIO( );
            if ( is_hovering( ) )
                if ( io.MouseClicked[ 0 ] && parent == nullptr ) {
                    parent = this;
                }

            if ( parent == this ) {
                *clicked = !*clicked;
                parent = nullptr;
            };
        };

        void on_pre_event( ) override {
            if ( *clicked )
                *clicked = !*clicked;
        };
    private:
        bool is_hovering( ) {
            auto io = ImGui::GetIO( );
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y && coords.x < position.x + size.x && coords.y < position.y + size.y );
        };
    private:
        vec2_t size;
        const char* childname;
        bool* clicked;
    };

    // const char* Title, const vec2_t Size, vec2_t Padding
    class c_groupbox : public c_controls {
    public:
        c_groupbox( const char* group_name, const vec2_t size = { 180.0f, 222.0f }, vec2_t pad = { 0.0f, 0.0f } ) {
            childname = group_name;
            group_size = size;
            group_padding = pad;
        };

        void on_render( ) override {
            if ( group_padding.x > 0.1f || group_padding.y > 0.1f )
                position += group_padding;
            const vec2_t groupbox_relative_position( position.x + 1.0f, position.y + 1.0f );
            const vec2_t groupbox_relative_size( group_size.x - 2.0f, group_size.y - 2.0f );
            Renderer::AddRect( { position.x, position.y }, { group_size.x, group_size.y }, ImColor( 0, 0, 0, 50 ) );
            Renderer::AddRectFilled( { groupbox_relative_position.x, groupbox_relative_position.y }, { groupbox_relative_size.x, groupbox_relative_size.y }, ImColor( 25, 27, 31, 255 ) ); // Background
            Renderer::AddRectFilled( ImVec2( groupbox_relative_position.x, groupbox_relative_position.y + 21.0f ), ImVec2( groupbox_relative_size.x, 1.0f ), menucolor ); // bar
            Renderer::AddRect( { groupbox_relative_position.x, groupbox_relative_position.y }, { groupbox_relative_size.x, groupbox_relative_size.y }, ImColor( 0, 0, 0, 255 ) );

            Renderer::AddText( ImVec2( groupbox_relative_position.x + 8.0f, groupbox_relative_position.y + 6.0f ), ImColor( 255, 255, 255, 255 ), childname );
            for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                it->get( )->on_render( );
            };
        };

        void on_post_event( ) override {
            for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                it->get( )->on_pre_event( );
                it->get( )->on_post_event( );
            };
        };

        void on_pre_event( ) override { };

        void add( std::unique_ptr< c_controls > control ) {
            control.get( )->set_position( { menu_position.x + 20.0f + ( padding.x + group_padding.x ), menu_position.y + 60.0f + ( padding.y + group_padding.y ) } );
            padding.y += control.get( )->get_padding( ).y;
            padding.x += control.get( )->get_padding( ).x;
            controls.push_back( std::move( control ) );
        };

        constexpr int control_amount( ) const { return controls.size( ); };
    private:
        std::vector< std::unique_ptr< c_controls > > controls;
        vec2_t padding;
        vec2_t group_padding;
        vec2_t group_size;
        const char* childname;
    };

    // const char* Title, const vec2_t Position
    class c_window {
    public:
        c_window( const char* title, const vec2_t position ) {
            menu_position = position;
            menu_size = { 384.0f, 282.0f };
            _title = title;
        };

        template< typename... args >
        void tabs( int* tabindex, args... tabs ) {
            const std::array< const char*, sizeof...( args ) > _tabs = { tabs... };
            const auto num_tabs = sizeof...( args );

            vec2_t menu_relative_position = menu_position + 1.0f;
            vec2_t menu_relative_size = menu_size - 2.0f;
            vec2_t button_size = { 60.0f, 20.0f };

            constexpr float tabpadding = 4.0f;

            auto io = ImGui::GetIO( );
            const auto cursor = io.MousePos;

            auto is_hovering = [ cursor ]( float x, float y, float w, float h ) {
                return ( cursor.x > x && cursor.y > y && cursor.x < x + w && cursor.y < y + h );
            };

            int next_index = 0;
            for ( auto op : _tabs ) {
                vec2_t button_position = vec2_t( ( menu_relative_position.x + menu_relative_size.x ) + next_index * ( button_size.x + tabpadding ), menu_relative_position.y );
                button_position.x -= ( button_size.x * num_tabs );
                Renderer::AddText( button_position + button_size / 2.0f, *tabindex == next_index ? menucolor : is_hovering( button_position.x, button_position.y, button_size.x, button_size.y ) ? menucolor : ImColor( 255, 255, 255, 255 ), op, true );

                if ( is_hovering( button_position.x, button_position.y, button_size.x, button_size.y ) ) {
                    if ( io.MouseClicked[ 0 ] ) {
                        *tabindex = next_index;
                    }
                };
                next_index++;
            };
        }

        void begin( ) {
            Renderer::AddRect( { menu_position.x, menu_position.y }, { menu_size.x, menu_size.y }, ImColor( 0, 0, 0, 150 ) );

            vec2_t menu_relative_position = menu_position + 1.0f;
            vec2_t menu_relative_size = menu_size - 2.0f;

            Renderer::AddRectFilled( menu_relative_position, menu_relative_size, ImColor( 25, 25, 25, 255 ) ); // Background
            Renderer::AddRectFilled( menu_relative_position, ImVec2( menu_relative_size.x, 20.0f ), ImColor( 23, 25, 28, 255 ) ); // Header
            Renderer::AddRectFilled( ImVec2( menu_relative_position.x, menu_relative_position.y + 20.0f ), ImVec2( menu_relative_size.x, 2.0f ), menucolor ); // Bar

            Renderer::AddText( ImVec2( menu_relative_position.x + 5.0f/*menu_relative_size.x - title_position.x - 10.0f*/, menu_relative_position.y + 5.0f ), ImColor( 255, 255, 255, 255 ), _title );

            // footer
            {
                const ImVec2 footer_position( menu_relative_position.x, menu_relative_position.y + menu_relative_size.y - 20.0f );
                const ImVec2 footer_size( menu_relative_size.x, 20.0f );

                Renderer::AddRectFilled( footer_position, footer_size, ImColor( 22, 23, 26, 255 ) );
                Renderer::AddLine( footer_position, ImVec2( footer_position.x + footer_size.x, footer_position.y ), ImColor( 17, 19, 22, 255 ) );

                const auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, "user:" );
                Renderer::AddText( ImVec2( footer_position.x + 10.0f, footer_position.y + 5.0f ), ImColor( 255, 255, 255, 255 ), "user:" );
                Renderer::AddText( ImVec2( footer_position.x + ( 12.0f + m_text_size.x ), footer_position.y + 5.0f ), menucolor, GetPCName( ) );

                const auto m_text_size2 = Renderer::g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, MUI_BUILD );
                Renderer::AddText( ImVec2( footer_position.x + footer_size.x - ( m_text_size2.x + 10.0f ), footer_position.y + 5.0f ), ImColor( 255, 255, 255, 255 ), MUI_BUILD );

                //MUI_BUILD
            };
        };

        void end( ) {
            for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                it->get( )->on_render( );
                it->get( )->on_post_event( );
            };
        };

        void add( std::unique_ptr< c_groupbox > control ) {
            control.get( )->set_position( { menu_position.x + 10.0f, menu_position.y + 30.0f + padding.y } );
            //padding.y += control.get( )->get_padding( ).y;
            controls.push_back( std::move( control ) );
        };

        vec2_t get_position( ) {
            return menu_position;
        };

        vec2_t get_size( ) {
            return menu_size;
        };
    private:
        std::vector< std::unique_ptr< c_groupbox > > controls;
        vec2_t padding;
        const char* _title;
    };
};
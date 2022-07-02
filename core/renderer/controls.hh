#pragma once
#include <windows.h>
#include <imgui/imgui.h>
#include <format>
#include <memory>
#include <array>
#include <string>
#include <algorithm>
#include <unordered_map>

#define menucolor ImColor( 255, 147, 183, 255 )
#define menucolor_darken ImColor( 255, 123, 154, 255 )
#define MUI_BUILD "Build date: " __DATE__ " @ " __TIME__

namespace Mui {
    char username_buffer[ 0xff ];
    ImGuiIO io = {};
    void SetupIO( ) {
        io = ImGui::GetIO( ); ( void )io;
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

        inline ImVec2 to_imvec( ) const noexcept { return ImVec2( x, y ); };

        inline operator ImVec2( ) {  return ImVec2( x, y ); };

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
        case VK_ESCAPE:
            return "[-]";
        case VK_SPACE:
            return "[SPACE]";
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
        virtual void on_pre_render( ) = 0;
        virtual void on_post_render( ) = 0;
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
        bool hotkey_drop;
        c_controls* control;
        float scrolldelta;
    };

    c_controls* parent = nullptr;
    std::unordered_map< const char*, childcontrols_t > children;

    // const char* Title, bool* Var
    class c_checkbox : public c_controls {
    public:
        c_checkbox( const char* name, bool* var ) {
            childname = name;
            padding = { 0.0f, 15.0f };
            size = { 8.0f, 8.0f };
            pvar = var;
        };

        void on_pre_render( ) override {
            Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y - 2.0f }, ImColor( 200, 200, 200, 255 ), childname );
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, *pvar ? menucolor : ImColor( 23, 23, 23, 255 ) );
        };

        void on_post_render( )override { };
        void on_post_event( ) override {
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

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos )
                Renderer::AddTextShadow( { position.x, position.y + 2.0f }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
            else
                Renderer::AddTextShadow( { position.x, position.y + 2.0f }, ImColor( 200, 200, 200, 255 ), childname );

            //UINT scanCode = MapVirtualKeyW( *pkey, MAPVK_VK_TO_VSC );
            //LONG lParamValue = ( scanCode << 16 );
            //char key_buffer[ 256 ];
            //GetKeyNameTextA( lParamValue, key_buffer, sizeof( key_buffer ) );
            //
            //auto buffer_data = std::format( "[{:s}]", children[ childname ].hotkey ? "..." : key_buffer );
            //const auto to_buffer = buffer_data.data( );
            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, to_buffer );
            Renderer::AddTextShadow( { position.x + size.x + ( 152.0f - m_text_size.x ), position.y }, is_hovering_key( ) ? menucolor : ImColor( 200, 200, 200, 255 ), to_buffer );
        };

        void on_post_event( ) override {
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

        void on_post_render( )override { };
        void on_pre_event( ) override { };
    private:
        bool is_hovering_key( ) {
            auto coords = io.MousePos;

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, to_buffer );
            auto frame_position = vec2_t{ position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f };
            return ( coords.x > frame_position.x && coords.y > frame_position.y && coords.x < frame_position.x + m_text_size.x && coords.y < frame_position.y + m_text_size.y );
        };
    private:
        const char* childname;
        int* pkey;
        vec2_t size;
    };

    class c_saturation_slider : public c_controls {
    public:
        c_saturation_slider( const char* name, float* value ) {
            childname = name;
            padding = { 0.0f, 20.0f };
            size = { 100.0f, 5.0f };
            pval = value;
            slider_pad_y = 10.0f;
            vmin = 0.0f;
            vmax = 1.0f;
            children[ childname ].control = this;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            char sliderbuff[ 128 ];
            if ( first != std::string::npos && second != std::string::npos ) {
                //Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
                sprintf_s( sliderbuff, 128, "%s (%.2f)", str_converted.erase( first, second ).c_str( ), *pval );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }
            else {
                sprintf_s( sliderbuff, 128, "%s (%.2f)", childname, *pval );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }


            Renderer::AddMultiColor( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 20, 20, 20, 255 ), ImColor( 150, 150, 150, 255 ), ImColor( 150, 150, 150, 255 ), ImColor( 20, 20, 20, 255 ) );
            Renderer::AddRect( { ( position.x + ( *pval * ( ( float )size.x / vmax ) - 3.0f ) ), position.y + slider_pad_y }, { 6.0f, size.y }, ImColor( 9, 10, 12, 255 ) );
            //Renderer::AddRect( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        }

        void on_post_render( )override { };
        void on_pre_event( ) override {}

        void on_post_event( ) override {
            if ( parent == nullptr ) {
                if ( is_hovering( ) ) {
                    if ( io.MouseClicked[ 0 ] && io.MouseDown[ 0 ] ) {
                        parent = children[ childname ].control;
                        if ( parent == children[ childname ].control )
                            children[ childname ].holding = true;;
                    };
                };
            };

            if ( parent != nullptr && children[ childname ].holding ) {
                if ( !io.MouseDown[ 0 ] ) {
                    parent = children[ childname ].control;
                    if ( parent == children[ childname ].control ) {
                        children[ childname ].holding = false;
                        parent = nullptr;
                    }
                };
            };

            if ( children[ childname ].holding && parent ) {
                if ( io.MouseDown[ 0 ] ) {
                    *pval = vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x;

                    *pval = std::clamp( *pval, vmin, vmax );
                }
                else {
                    children[ childname ].holding = false;
                    parent = nullptr;
                };
            };
        };
    private:
        bool is_hovering( ) {
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y + slider_pad_y && coords.x < position.x + size.x && coords.y < ( position.y + slider_pad_y ) + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };
    private:
        const char* childname;
        vec2_t size;
        float* pval;
        float vmin;
        float vmax;
        float slider_pad_y;
    };

    class c_slider_float : public c_controls {
    public:
        c_slider_float( const char* name, float* value, float min, float max, const char* delim = "" ) {
            childname = name;
            padding = { 0.0f, 20.0f };
            size = { 100.0f, 5.0f };
            vmin = min;
            vmax = max;
            pval = value;
            szdelim = delim;
            slider_pad_y = 10.0f;

            children[ childname ].control = this;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            char sliderbuff[ 128 ];
            if ( first != std::string::npos && second != std::string::npos ) {
                //Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
                sprintf_s( sliderbuff, 128, "%s (%.2f%s)", str_converted.erase( first, second ).c_str( ), *pval, szdelim );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }
            else {
                sprintf_s( sliderbuff, 128, "%s (%.2f%s)", childname, *pval, szdelim );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }

            Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );

            if ( is_hovering( ) && !children[ childname ].holding ) {
                auto temp_val = vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x;
                Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { temp_val * ( ( float )size.x / vmax ), size.y }, menucolor_darken );
            }
            Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { *pval * ( ( float )size.x / vmax ), size.y }, menucolor );
            //Renderer::AddRect( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        };

        void on_post_render( )override { };
        void on_pre_event( ) override {}

        void on_post_event( ) override {
            if ( parent == nullptr ) {
                if ( is_hovering( ) ) {
                    if ( io.MouseClicked[ 0 ] && io.MouseDown[ 0 ] ) {
                        parent = children[ childname ].control;
                        if ( parent == children[ childname ].control )
                            children[ childname ].holding = true;;
                    };
                };
            };

            if ( parent != nullptr && children[ childname ].holding ) {
                if ( !io.MouseDown[ 0 ] ) {
                    parent = children[ childname ].control;
                    if ( parent == children[ childname ].control ) {
                        children[ childname ].holding = false;
                        parent = nullptr;
                    }
                };
            };

            if ( children[ childname ].holding && parent ) {
                if ( io.MouseDown[ 0 ] ) {
                    *pval = vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x;

                    *pval = std::clamp( *pval, vmin, vmax );
                }
                else {
                    children[ childname ].holding = false;
                    parent = nullptr;
                };
            };
        };
    private:
        bool is_hovering( ) {
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y + slider_pad_y && coords.x < position.x + size.x && coords.y < ( position.y + slider_pad_y ) + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };
    private:
        const char* childname;
        vec2_t size;
        float* pval;
        float vmin;
        float vmax;
        const char* szdelim;
        float slider_pad_y;
    };

    class c_color_picker : public c_controls {
    public:
        c_color_picker( const char* name, float* value, float sat = 1.0f ) {
            childname = name;
            padding = { 0.0f, 20.0f };
            size = { 100.0f, 5.0f };
            vmin = 0.0f;
            vmax = 1.0f;
            pval = value;
            _sat = sat;
            slider_pad_y = 10.0f;
            children[ childname ].control = this;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            char sliderbuff[ 128 ];
            if ( first != std::string::npos && second != std::string::npos ) {
                //Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
                sprintf_s( sliderbuff, 128, "%s (%.2f)", str_converted.erase( first, second ).c_str( ), *pval );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }
            else {
                sprintf_s( sliderbuff, 128, "%s (%.2f)", childname, *pval );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }

            ImColor m_hue_colors[ 7 ] = {
                ImColor( 255, 0, 0, 255 ),
                ImColor( 255, 255, 0, 255 ),
                ImColor( 0, 255, 0, 255 ),
                ImColor( 0, 255, 255, 255 ),
                ImColor( 0, 0, 255, 255 ),
                ImColor( 255, 0, 255, 255 ),
                ImColor( 255, 0, 0, 255 )
            };
            const auto rgb_width = size.x / 6;

            for ( int i = 0; i < 6; i++ )
                Renderer::AddMultiColor( { position.x + i * rgb_width, position.y + slider_pad_y }, { rgb_width, size.y }, m_hue_colors[ i ], m_hue_colors[ i + 1 ], m_hue_colors[ i + 1 ], m_hue_colors[ i ] );

            Renderer::AddRectFilled( { position.x + size.x + 10.0f, position.y + slider_pad_y }, { size.y + 10.0f, size.y }, ImColor::HSV( *pval, _sat, 1.0f ) );

            if ( is_hovering( ) && !children[ childname ].holding ) {
                auto temp_val = vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x;
                //Renderer::AddRectFilled( { ( position.x + temp_val * ( ( float )size.x / vmax ) - 4.0f ), position.y }, { 6.0f, size.y }, { 150, 50, 50, 180 } );
            }

            // Before you ask: "Why are you rendering another rect, you could just use the outline, right?"
            // And to my answer: The result would get "gradient"ed.
            Renderer::AddRectFilled( { ( position.x + ( *pval * ( ( float )size.x / vmax ) - 3.0f ) ), position.y + slider_pad_y }, { 6.0f, size.y }, ImColor::HSV( *pval, _sat, 1.0f ) );
            Renderer::AddRect( { ( position.x + ( *pval * ( ( float )size.x / vmax ) - 3.0f ) ), position.y + slider_pad_y }, { 6.0f, size.y }, ImColor( 9, 10, 12, 255 ) );
        }

        void on_post_render( )override { };
        void on_pre_event( ) override {}

        void on_post_event( ) override {
            if ( parent == nullptr ) {
                if ( is_hovering( ) ) {
                    if ( io.MouseClicked[ 0 ] && io.MouseDown[ 0 ] ) {
                        parent = children[ childname ].control;
                        if ( parent == children[ childname ].control )
                            children[ childname ].holding = true;;
                    };
                };
            };

            if ( parent != nullptr && children[ childname ].holding ) {
                if ( !io.MouseDown[ 0 ] ) {
                    parent = children[ childname ].control;
                    if ( parent == children[ childname ].control ) {
                        children[ childname ].holding = false;
                        parent = nullptr;
                    }
                };
            };

            if ( children[ childname ].holding && parent ) {
                if ( io.MouseDown[ 0 ] ) {
                    *pval = static_cast< float >( vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x );

                    *pval = std::clamp( *pval, vmin, vmax );
                }
                else {
                    children[ childname ].holding = false;
                    parent = nullptr;
                };
            };
        };
    private:
        bool is_hovering( ) {
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y + slider_pad_y && coords.x < position.x + size.x && coords.y < position.y + slider_pad_y + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };
    private:
        const char* childname;
        vec2_t size;
        float* pval;
        float vmin;
        float vmax;
        float slider_pad_y;
        float _sat;
    };

    class c_slider_int : public c_controls {
    public:
        c_slider_int( const char* name, int* value, int min, int max, const char* delim = "" ) {
            childname = name;
            padding = { 0.0f, 20.0f };
            size = { 100.0f, 5.0f };
            vmin = min;
            vmax = max;
            pval = value;
            szdelim = delim;
            slider_pad_y = 10.0f;
            children[ childname ].control = this;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            char sliderbuff[ 128 ];
            if ( first != std::string::npos && second != std::string::npos ) {
                //Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
                sprintf_s( sliderbuff, 128, "%s (%i%s)", str_converted.erase( first, second ).c_str( ), *pval, szdelim );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }
            else {
                sprintf_s( sliderbuff, 128, "%s (%i%s)", childname, *pval, szdelim );
                Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), sliderbuff );
            }

            Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );

            if ( is_hovering( ) && !children[ childname ].holding ) {
                auto temp_val = vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x;
                Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { temp_val * ( ( float )size.x / vmax ), size.y }, menucolor_darken );
            }
            Renderer::AddRectFilled( { position.x, position.y + slider_pad_y }, { *pval * ( ( float )size.x / vmax ), size.y }, menucolor );
            //Renderer::AddRect( { position.x, position.y + slider_pad_y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        }

        void on_post_render( )override { };
        void on_pre_event( ) override {}

        void on_post_event( ) override {
            if ( parent == nullptr ) {
                if ( is_hovering( ) ) {
                    if ( io.MouseClicked[ 0 ] && io.MouseDown[ 0 ] ) {
                        parent = children[ childname ].control;
                        if ( parent == children[ childname ].control )
                            children[ childname ].holding = true;;
                    };
                };
            };

            if ( parent != nullptr && children[ childname ].holding ) {
                if ( !io.MouseDown[ 0 ] ) {
                    parent = children[ childname ].control;
                    if ( parent == children[ childname ].control ) {
                        children[ childname ].holding = false;
                        parent = nullptr;
                    }
                };
            };

            if ( children[ childname ].holding && parent ) {
                if ( io.MouseDown[ 0 ] ) {
                    *pval = static_cast< int >( vmin + ( vmax - vmin ) * ( io.MousePos.x - position.x ) / size.x );

                    *pval = std::clamp( *pval, vmin, vmax );
                }
                else {
                    children[ childname ].holding = false;
                    parent = nullptr;
                };
            };
        };
    private:
        bool is_hovering( ) {
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y + slider_pad_y && coords.x < position.x + size.x && coords.y < ( position.y + slider_pad_y ) + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };
    private:
        const char* childname;
        vec2_t size;
        int* pval;
        int vmin;
        int vmax;
        const char* szdelim;
        float slider_pad_y;
    };

    // const char* Title, bool* Var, int* Key
    class c_checkbox_hotkey : public c_controls {
    public:
        c_checkbox_hotkey( const char* name, bool* var, int* key ) {
            childname = name;
            padding = { 0.0f, 15.0f };
            size = { 8.0f, 8.0f };
            pvar = var;
            pkey = key;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos )
                Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
            else
                Renderer::AddTextShadow( { position.x + size.x + 4.0f, position.y }, ImColor( 200, 200, 200, 255 ), childname );
            
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, *pvar ? menucolor : ImColor( 23, 23, 23, 255 ) );

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, to_buffer );
            Renderer::AddTextShadow( { position.x + size.x + ( 152.0f - m_text_size.x ), position.y }, is_hovering_key( ) ? menucolor : ImColor( 200, 200, 200, 255 ), to_buffer );
        };

        void on_post_event( ) override {
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

        void on_post_render( )override { };
        void on_pre_event( ) override {

        };
    private:
        bool is_hovering( ) {
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y && coords.x < position.x + size.x && coords.y < position.y + size.y );
        };

        bool is_hovering_key( ) {
            auto coords = io.MousePos;

            auto to_buffer = children[ childname ].hotkey ? "[...]" : get_key_name( pkey );
            auto m_text_size = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, to_buffer );
            auto frame_position = vec2_t{ position.x + size.x + ( 152.0f - m_text_size.x ), position.y + 2.0f };
            return ( coords.x > frame_position.x && coords.y > frame_position.y - 1 && coords.x < frame_position.x + m_text_size.x && coords.y < frame_position.y + m_text_size.y - 2 );
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
            size = { 100.0f, 12.0f };
            padding = { 0.0f, 25.0f };
            m_items = items;
            m_items_size = items_size;
            selected_index = selected;
            m_combo_pad_y = 10.0f;
            children[ childname ].control = this;
        };

        void on_pre_render( ) override {
            auto str_converted = std::string( childname );
            std::size_t first = str_converted.find_first_of( "#" );
            std::size_t second = str_converted.find_last_of( "#" );

            if ( first != std::string::npos && second != std::string::npos )
                Renderer::AddTextShadow( { position.x, position.y - 2.0f }, ImColor( 200, 200, 200, 255 ), str_converted.erase( first, second ).c_str( ) );
            else
                Renderer::AddTextShadow( { position.x, position.y - 2.0f }, ImColor( 200, 200, 200, 255 ), childname );
            
            Renderer::AddRectFilled( { position.x, position.y + m_combo_pad_y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
            Renderer::AddTextShadow( { position.x + 6.0f, position.y + m_combo_pad_y + 1.0f }, children[ childname ].opened ? menucolor : ImColor( 200, 200, 200, 255 ), m_items[ *selected_index ] );
            Renderer::AddTextShadow( { position.x + size.x - 6.0f, position.y + m_combo_pad_y + 1.0f }, children[ childname ].opened ? menucolor : ImColor( 200, 200, 200, 255 ), children[ childname ].opened ? "-" : "+" );
        };

        void on_post_render( ) override {
            // render all the items
            if ( !children[ childname ].opened )
                return;

            for ( int item_next_index = 0; item_next_index < m_items_size; item_next_index++ ) {
                auto current_item = m_items[ item_next_index ];
                if ( !current_item )
                    continue;

                bool is_hover = is_hovering_custom( position.x, position.y + m_combo_pad_y + size.y + item_next_index * size.y, size.x, size.y );
                Renderer::AddRectFilled( { position.x, position.y + m_combo_pad_y + size.y + item_next_index * size.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
                Renderer::AddTextShadow( { position.x + 6.0f, position.y + m_combo_pad_y + size.y + item_next_index * size.y }, is_hover ? menucolor : ImColor( 200, 200, 200, 255 ), current_item );
            };
        };


        void on_post_event( ) override {
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
                bool is_outside = !is_hovering_custom( position.x, position.y + m_combo_pad_y, size.x, size.y + m_items_size * size.y );
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

                    if ( !is_hovering_custom( position.x, position.y + m_combo_pad_y + size.y + item_next_index * size.y, size.x, size.y ) )
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
            auto coords = io.MousePos;
            return ( coords.x > position.x && coords.y > position.y + m_combo_pad_y && coords.x < position.x + size.x && coords.y < position.y + m_combo_pad_y + size.y );
        };

        bool is_hovering_custom( float x, float y, float w, float h ) {
            auto coords = io.MousePos;
            return ( coords.x > x && coords.y > y && coords.x < x + w && coords.y < y + h );
        };

    private:
        int* selected_index;
        const char* childname;
        vec2_t size;
        const char* const* m_items;
        int m_items_size;
        float m_combo_pad_y;
    };

    // const char* Title
    class c_label : public c_controls {
    public:
        c_label( const char* name ) {
            label = name;
            padding = { 0.0f, 15.0f };
        };

        void on_pre_render( ) override {
            Renderer::AddTextShadow( { position.x, position.y }, ImColor( 200, 200, 200, 255 ), label );
        };

        void on_post_render( )override { };
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

        void on_pre_render( ) override {
            if ( show_border )
                Renderer::AddLine( { position.x, position.y + ( padding.y / 2.0f ) }, { position.x + 90.0f, position.y + ( padding.y / 2.0f ) }, ImColor( 255, 255, 255, 255 ) );
        };

        void on_post_render( )override { };
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
                const auto text_size = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, childname );
                constexpr float selfpad = 6.0f;
                size = { text_size.x + selfpad, text_size.y + selfpad };
                padding.y += selfpad;
            }
            else {
                padding.y += size.y;
            };
        };

        void on_post_render( )override { };
        void on_pre_render( ) override {
            Renderer::AddRectFilled( { position.x, position.y }, { size.x, size.y }, ImColor( 23, 23, 23, 255 ) );
            Renderer::AddTextShadow( position + size / 2.0f, is_hovering( ) ? menucolor : ImColor( 255, 255, 255, 255 ), childname, true );
            Renderer::AddRect( { position.x, position.y }, { size.x, size.y }, ImColor( 9, 10, 12, 255 ) );
        };

        void on_post_event( ) override {
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

        void on_post_render( )override { };
        void on_pre_render( ) override {
            if ( group_padding.x > 0.1f || group_padding.y > 0.1f )
                position += group_padding;
            const vec2_t groupbox_relative_position( position.x + 1.0f, position.y + 1.0f );
            const vec2_t groupbox_relative_size( group_size.x - 2.0f, group_size.y - 2.0f );

            Renderer::AddRectFilled( { groupbox_relative_position.x, groupbox_relative_position.y }, { groupbox_relative_size.x, groupbox_relative_size.y }, ImColor( 30, 30, 30, 255 ) ); // Background
            Renderer::AddRectFilled( ImVec2( groupbox_relative_position.x, groupbox_relative_position.y ), ImVec2( groupbox_relative_size.x, 20.0f ), ImColor( 40, 40, 40, 255 ) ); // bar

            auto fade_bar_size = groupbox_relative_size.x / 2.0f;
            Renderer::AddMultiColor( { groupbox_relative_position.x, groupbox_relative_position.y + 20.0f }, { fade_bar_size, 2.0f }, ImColor( 0, 0, 0, 0 ), is_within_groupbox( ) ? ImColor( 200, 200, 200, 255 ) : menucolor, is_within_groupbox( ) ? ImColor( 200, 200, 200, 255 ) : menucolor, ImColor( 0, 0, 0, 0 ) );
            Renderer::AddMultiColor( { groupbox_relative_position.x + fade_bar_size, groupbox_relative_position.y + 20.0f }, { fade_bar_size, 2.0f }, is_within_groupbox( ) ? ImColor( 200, 200, 200, 255 ) : menucolor, ImColor( 0, 0, 0, 0 ), ImColor( 0, 0, 0, 0 ), is_within_groupbox( ) ? ImColor( 200, 200, 200, 255 ) : menucolor );

            Renderer::AddTextShadow( ImVec2( groupbox_relative_position.x + 8.0f, groupbox_relative_position.y + 6.0f ), ImColor( 200, 200, 200, 255 ), childname );

            if ( scrollbar_requirement( ) ) {
                const float result_height = padding.y - group_size.y;
                const float total_height = group_size.y - result_height;

                float abs_delta = group_size.y * ( children[ childname ].scrolldelta / padding.y );
                float constant = group_size.y * ( group_size.y - 80.0f ) / padding.y;
                //AddRectFilled( { pos.x + size.x - 4.0f, pos.y + abs_delta }, { 4.0f, constant }, ImColor( 255, 25, 25, 255 ) );

                if ( is_within_groupbox( ) ) {
                    if ( io.MouseWheel > 0.0f )
                        children[ childname ].scrolldelta -= 6.0f;

                    if ( io.MouseWheel < 0.0f )
                        children[ childname ].scrolldelta += 6.0f;

                    if ( children[ childname ].scrolldelta < 0.0f )
                        children[ childname ].scrolldelta = 0.0f;

                    if ( children[ childname ].scrolldelta > ( padding.y - constant - 20.0f ) )
                        children[ childname ].scrolldelta = ( padding.y - constant - 20.0f );
                };

                Renderer::PushClip( { groupbox_relative_position.x, groupbox_relative_position.y + 25.0f }, { groupbox_relative_size.x, groupbox_relative_size.y - 25.0f } );
                Renderer::AddRectFilled( { groupbox_relative_position.x + group_size.x - 4.0f, groupbox_relative_position.y + abs_delta + 20.0f }, { 2.0f, constant - 4.0f }, menucolor );
                for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                    it->get( )->position.y -= children[ childname ].scrolldelta;
                    it->get( )->on_pre_render( );
                };
                Renderer::PopClip( );

                for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                    it->get( )->on_post_render( );
                };
            }
            else {
                for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                    it->get( )->on_pre_render( );
                    it->get( )->on_post_render( );
                };
            }
        };

        void on_post_event( ) override {
            if ( scrollbar_requirement( ) ) {
                for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                    it->get( )->on_pre_event( );
                };
                if ( is_within_groupbox( ) ) {
                    for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                        it->get( )->on_post_event( );
                    };
                };
            }
            else {
                for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                    it->get( )->on_pre_event( );
                    it->get( )->on_post_event( );
                };
            };
        };

        void on_pre_event( ) override { };

        void add( std::unique_ptr< c_controls > control ) {
            control.get( )->set_position( { menu_position.x + 20.0f + ( padding.x + group_padding.x ), menu_position.y + 70.0f + ( padding.y + group_padding.y ) } );
            padding.y += control.get( )->get_padding( ).y;
            padding.x += control.get( )->get_padding( ).x;
            controls.push_back( std::move( control ) );
        };

        constexpr int control_amount( ) const { return controls.size( ); };

        constexpr bool scrollbar_requirement( ) const { return padding.y + 20.0f > group_size.y; };

        bool is_within_groupbox( ) {
            return ( io.MousePos.x > position.x && io.MousePos.y > position.y && io.MousePos.x < position.x + group_size.x && io.MousePos.y < position.y + group_size.y );
        };
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
        c_window( const char* title, const vec2_t position, const char* second_title = "" ) {
            menu_position = position;
            menu_size = { 384.0f, 400.0f };
            _title = title;
            _second = second_title;
            SetupIO( );
        };

        template< typename... args >
        void tabs( int* tabindex, args... tabs ) {
            const std::array< const char*, sizeof...( args ) > _tabs = { tabs... };
            const auto num_tabs = sizeof...( args );

            const auto cursor = io.MousePos;

            auto is_hovering = [ cursor ]( float x, float y, float w, float h ) {
                return ( cursor.x > x && cursor.y > y && cursor.x < x + w && cursor.y < y + h );
            };
            vec2_t tab_absolute_position = { menu_position.x + menu_size.x - 155.0f, menu_position.y + 12.0f };
            for ( std::size_t i = 0; i != _tabs.size( ); ++i ) {
                auto op = _tabs[ i ];
                if ( !op )
                    continue;

                auto text_strip = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, op );
                const auto is_hovering_at_location = is_hovering( tab_absolute_position.x, tab_absolute_position.y, text_strip.x, text_strip.y );

                //Renderer::AddRectFilled( { tab_absolute_position.x, menu_size.y }, { text_strip.x, text_strip.y }, *tabindex == t ? menucolor : ImColor( 200, 200, 200, 255 ) );
                Renderer::AddTextShadow( { tab_absolute_position.x, tab_absolute_position.y }, *tabindex == i ? menucolor : is_hovering_at_location ? menucolor : ImColor( 200, 200, 200, 255 ), op, false );

                if ( is_hovering_at_location )
                    if ( io.MouseClicked[ 0 ] )
                        *tabindex = i;

                tab_absolute_position.x += text_strip.x + 6.0f;
            };
        }

        void begin( ) {
            Renderer::AddRectFilled( menu_position, menu_size, ImColor( 15, 15, 15, 255 ) ); // Background
            Renderer::AddRectFilled( { menu_position.x + 4.0f, menu_position.y + 4.0f }, ImVec2( menu_size.x - 8.0f, 30.0f ), ImColor( 30, 30, 30, 255 ) ); // Header
            Renderer::AddRect( menu_position, menu_size, ImColor( 0, 0, 0, 150 ) );

            // alternative:
            //Renderer::AddRectFilled( { menu_position.x + 1.0f, menu_position.y + 1.0f }, { 2.0f, menu_size.y - 2.0f }, menucolor );

            const auto menu_bar_size = menu_size.y / 2.0f;
            Renderer::AddMultiColor( { menu_position.x + 1.0f, menu_position.y + 1.0f }, { 2.0f, menu_bar_size - 2.0f }, ImColor( 0, 0, 0, 0 ), ImColor( 0, 0, 0, 0 ), menucolor, menucolor );
            Renderer::AddMultiColor( { menu_position.x + 1.0f, menu_position.y + menu_bar_size - 1.0f }, { 2.0f, menu_bar_size - 2.0f }, menucolor, menucolor, ImColor( 0, 0, 0, 0 ), ImColor( 0, 0, 0, 0 ) );

            Renderer::AddText( ImVec2( menu_position.x + 8.0f/*menu_relative_size.x - title_position.x - 10.0f*/, menu_position.y + 7.0f ), ImColor( 200, 200, 200, 255 ), _title );
            Renderer::AddText( ImVec2( menu_position.x + 8.0f/*menu_relative_size.x - title_position.x - 10.0f*/, menu_position.y + 17.0f ), menucolor, _second );

        };

        void end( ) {
            for ( auto it = controls.rbegin( ); it != controls.rend( ); it++ ) {
                it->get( )->on_pre_render( );
                it->get( )->on_post_event( );
            };
        };

        void debug( ) {
            Renderer::AddRectFilled( { 0.0f, 0.0f }, { 1920.0f, 1080.0f }, ImColor( 79, 147, 183, 255 ) );
        };

        void add( std::unique_ptr< c_groupbox > control ) {
            control.get( )->set_position( { menu_position.x + 10.0f, menu_position.y + 40.0f + padding.y } );
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
        const char* _second;
    };
};

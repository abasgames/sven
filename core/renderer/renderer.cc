#include "renderer.hh"

ImFont* Renderer::g_pFont = nullptr;
ImDrawList* Renderer::g_pDrawList = nullptr;

void Renderer::Setup( ImDrawList* list ) {
    g_pDrawList = list;
}

void Renderer::AddText( ImVec2 position, ImColor color, const char* text, bool center ) {
    if ( center ) {
        auto text_size = g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( g_pFont, 8.0f, position, color, text );
};

void Renderer::AddRect( const ImVec2 position, const ImVec2 size, ImColor color ) {
    g_pDrawList->AddRect( position, ImVec2( position.x + size.x, position.y + size.y ), color );
};

void Renderer::AddRectFilled( const ImVec2 position, const ImVec2 size, ImColor color ) {
    g_pDrawList->AddRectFilled( position, ImVec2( position.x + size.x, position.y + size.y ), color );
};

void Renderer::AddTextOutline( ImVec2 position, ImColor color, const char* text, bool center ) {
    if ( center ) {
        auto text_size = g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( g_pFont, 8.0f, ImVec2( position.x - 1.0f, position.y - 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( g_pFont, 8.0f, ImVec2( position.x + 1.0f, position.y - 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( g_pFont, 8.0f, ImVec2( position.x - 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( g_pFont, 8.0f, ImVec2( position.x + 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( g_pFont, 8.0f, position, color, text );
}
void Renderer::AddRectOutline( const ImVec2 position, const ImVec2 size, ImColor color )
{
    AddRect( { position.x + 1.0f, position.y + 1.0f }, { size.x - 2.0f, size.y - 2.0f }, ImColor( 0, 0, 0, 255 ) );
    AddRect( { position.x - 1.0f, position.y - 1.0f }, { size.x + 2.0f, size.y + 2.0f }, ImColor( 0, 0, 0, 255 ) );
    AddRect( position, size, color );
};

void Renderer::AddLine( const ImVec2 pos0, const ImVec2 pos1, ImColor color ) {
    g_pDrawList->AddLine( pos0, pos1, color );
};

void Renderer::AddTextShadow( ImVec2 position, ImColor color, const char* text, bool center ) {
    if ( center ) {
        auto text_size = g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( g_pFont, 8.0f, ImVec2( position.x + 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( g_pFont, 8.0f, position, color, text );
};

void Renderer::AddTab( const char* item ) {
    const ImVec2 fixed_position( ImVec2( 10.0f, 10.0f ) );
    const ImVec2 fixed_size( ImVec2( 120.0f, 60.0f ) );

    ImVec2 text_position = fixed_position;
    auto font_text = g_pFont->CalcTextSizeA( 8.0f, FLT_MAX, 0.0f, item );
    text_position.x -= font_text.x / 2;
    text_position.y -= font_text.y / 2;

    AddRectFilled( fixed_position, fixed_size, ImColor( 25, 25, 25, 255 ) );
    AddTextShadow( ImVec2( text_position.x + fixed_size.x / 2.0f, text_position.y + fixed_size.y / 2.0f ), ImColor( 255, 255, 255, 255 ), item, false );
};

void Renderer::AddPoly( ImVec2* polys, int points, ImColor color ) {
    g_pDrawList->AddConvexPolyFilled( polys, points, color );
};
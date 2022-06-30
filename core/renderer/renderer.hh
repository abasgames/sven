#pragma once
#include <imgui/imgui.h>
namespace Renderer {
    extern ImFont* g_pFont;
    extern ImFont* g_pGilroy;
    extern ImDrawList* g_pDrawList;

    void Setup( ImDrawList* drawlist );

    void AddText( ImVec2 position, ImColor color, const char* text, bool center = false, ImFont* font = g_pFont, float font_size = 8.0f );

    void AddRect( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddRectFilled( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddTextOutline( ImVec2 position, ImColor color, const char* text, bool center = false, ImFont* font = g_pFont, float font_size = 8.0f );

    void AddRectOutline( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddLine( const ImVec2 pos0, const ImVec2 pos1, ImColor color );

    void AddTextShadow( ImVec2 position, ImColor color, const char* text, bool center = false, ImFont* font = g_pFont, float font_size = 8.0f );

    void AddMultiColor( const ImVec2 position, const ImVec2 size, ImColor color0, ImColor color1, ImColor color2, ImColor color3 );

    void AddTab( const char* item );

    void AddPoly( ImVec2* polys, int points, ImColor color );

    void PushNotification( const char* a0, const char* a1, const char* a2, float delay = 4000.0f, ImColor color = ImColor( 255, 147, 183, 255 ) );

    void RenderNotifications( );

    int NotificationSize( );

    void PushClip( const ImVec2 position, const ImVec2 size );

    void PopClip( );
};
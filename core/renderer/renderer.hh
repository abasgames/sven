#pragma once
#include <imgui/imgui.h>
namespace Renderer {
    extern ImFont* g_pFont;
    extern ImDrawList* g_pDrawList;

	void Setup( ImDrawList* drawlist );

    void AddText( ImVec2 position, ImColor color, const char* text, bool center = false );

    void AddRect( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddRectFilled( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddTextOutline( ImVec2 position, ImColor color, const char* text, bool center = false );

    void AddRectOutline( const ImVec2 position, const ImVec2 size, ImColor color );

    void AddLine( const ImVec2 pos0, const ImVec2 pos1, ImColor color );

    void AddTextShadow( ImVec2 position, ImColor color, const char* text, bool center = false );

    void AddTab( const char* item );

    void AddPoly( ImVec2* polys, int points, ImColor color );
};
#pragma once
#include <imgui/imgui.h>
namespace api {
	void AddText( ImDrawList* list, const char* text, ImVec2 pos ) {
		list->AddText( pos, ImColor( 255, 255, 255, 255 ), text );
	};

	void AddRect( ImDrawList* list, float x, float y, float w, float h, ImColor color ) {
		auto m_w = x + w;
		auto m_h = y + h;
		list->AddRect( ImVec2( x, y ), ImVec2( m_w, m_h ), color );
	};
};
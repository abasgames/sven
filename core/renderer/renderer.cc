#include <windows.h>
#include "renderer.hh"
#include <vector>
ImFont* Renderer::g_pFont = nullptr;
ImFont* Renderer::g_pGilroy = nullptr;
ImDrawList* Renderer::g_pDrawList = nullptr;

void Renderer::Setup( ImDrawList* list ) {
    g_pDrawList = list;
}

void Renderer::AddText( ImVec2 position, ImColor color, const char* text, bool center, ImFont* font, float font_size ) {
    if ( center ) {
        auto text_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( font, font_size, position, color, text );
};

void Renderer::AddRect( const ImVec2 position, const ImVec2 size, ImColor color ) {
    g_pDrawList->AddRect( position, ImVec2( position.x + size.x, position.y + size.y ), color );
};

void Renderer::AddRectFilled( const ImVec2 position, const ImVec2 size, ImColor color ) {
    g_pDrawList->AddRectFilled( position, ImVec2( position.x + size.x, position.y + size.y ), color );
};

void Renderer::AddTextOutline( ImVec2 position, ImColor color, const char* text, bool center, ImFont* font, float font_size ) {
    if ( center ) {
        auto text_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( font, font_size, ImVec2( position.x - 1.0f, position.y - 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( font, font_size, ImVec2( position.x + 1.0f, position.y - 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( font, font_size, ImVec2( position.x - 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( font, font_size, ImVec2( position.x + 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( font, font_size, position, color, text );
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

void Renderer::AddTextShadow( ImVec2 position, ImColor color, const char* text, bool center, ImFont* font, float font_size ) {
    if ( center ) {
        auto text_size = font->CalcTextSizeA( font_size, FLT_MAX, 0.0f, text );
        auto wide = text_size.x;
        auto tall = text_size.y;

        position.x -= wide / 2;
        position.y -= tall / 2;
    };
    g_pDrawList->AddText( font, font_size, ImVec2( position.x + 1.0f, position.y + 1.0f ), ImColor( 0, 0, 0, 255 ), text );
    g_pDrawList->AddText( font, font_size, position, color, text );
};

void Renderer::AddMultiColor( const ImVec2 position, const ImVec2 size, ImColor color0, ImColor color1, ImColor color2, ImColor color3 ) {
    g_pDrawList->AddRectFilledMultiColor( position, ImVec2( position.x + size.x, position.y + size.y ), color0, color1, color2, color3 );
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
}

float lerp( float a, float b, float t ) {
	return a + ( b - a ) * t;
};

struct notification_t {
	bool lerp_foreground;
	bool lerp_background;
	bool lerp_backwards;
	bool lerp_complete;
	bool lerp_full_completion;

	float lerp_foreground_width;
	float lerp_background_width;
	float lerp_ease_out;

	const char* text_one;
	const char* text_two;
	const char* text_three;

	float next_notification_padding;
	ImColor notification_color;

	notification_t( const char* _text1, const char* _text2, const char* _text3, float delay = 4000.0f, ImColor color = ImColor( 255, 147, 183, 255 ) )
		: text_one{ _text1 }, text_two{ _text2 }, text_three{ _text3 } {
		lerp_foreground = true;
		lerp_background = false;
		lerp_backwards = false;
		lerp_complete = false;
		lerp_full_completion = false;
		lerp_foreground_width = 0.0f;
		lerp_background_width = 0.0f;
		lerp_ease_out = static_cast< float >( GetTickCount64( ) ) + delay;
		notification_color = color;
	};

	constexpr float time( ) const { return lerp_ease_out; };

	bool dead( ) { return lerp_full_completion; };

	void draw( ) {
		const auto t0 = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, text_one );
		const auto t1 = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, text_two );
		const auto t2 = Renderer::g_pFont->CalcTextSizeA( 10.0f, FLT_MAX, 0.0f, text_three );

		const float text_total_width = ( t0.x + t1.x + t2.x ) + 16.0f;
		auto text_position_x = -text_total_width;

		if ( !lerp_full_completion ) {
			if ( lerp_foreground && !lerp_backwards ) {
				lerp_foreground_width = lerp( lerp_foreground_width, text_total_width, ImGui::GetIO( ).Framerate * 0.0002f );
			};

			if ( lerp_foreground && lerp_foreground_width >= text_total_width - 1.0f ) {
				lerp_foreground = false;
				lerp_background = true;
				lerp_foreground_width = text_total_width;
			};

			if ( lerp_background && !lerp_backwards ) {
				lerp_background_width = lerp( lerp_background_width, text_total_width, ImGui::GetIO( ).Framerate * 0.0002f );
			};

			if ( lerp_background && lerp_background_width >= text_total_width - 1.0f ) {
				lerp_background = false;
				lerp_background = text_total_width;
				lerp_complete = true;
			};

			if ( lerp_complete ) {
				if ( static_cast< float >( GetTickCount64( ) ) >= time( ) ) {
					lerp_backwards = true;
				};

				if ( lerp_backwards ) {
					lerp_background_width = lerp( lerp_background_width, 0.0f, ImGui::GetIO( ).Framerate * 0.0002f );

					if ( lerp_background_width <= 1.0f ) {
						lerp_foreground = true;
						lerp_background_width = 0.0f;
					}

					if ( lerp_foreground )
						lerp_foreground_width = lerp( lerp_foreground_width, 0.0f, ImGui::GetIO( ).Framerate * 0.0002f );

					if ( lerp_foreground_width <= 1.0f ) {
						lerp_foreground = false;
						lerp_foreground_width = 0.0f;
						lerp_full_completion = true;
					};
				};
			};

			Renderer::AddRectFilled( { 0.0f, next_notification_padding }, { lerp_foreground_width, 20.0f }, notification_color );
			Renderer::AddRectFilled( { 0.0f, next_notification_padding }, { lerp_background_width, 20.0f }, ImColor( 15, 15, 15, 255 ) );

			const auto lerp_text = text_position_x + lerp_background_width;
			Renderer::AddText( { lerp_text + 8.0f, next_notification_padding + 6.0f }, ImColor( 200, 200, 200, 255 ), text_one );
			Renderer::AddText( { lerp_text + 8.0f + ( t0.x ), next_notification_padding + 6.0f }, notification_color, text_two );
			Renderer::AddText( { lerp_text + 8.0f + ( t0.x + t1.x ), next_notification_padding + 6.0f }, ImColor( 200, 200, 200, 255 ), text_three );
		};
	};
};

std::vector< notification_t > notifications{ };

void Renderer::PushNotification( const char* a0, const char* a1, const char* a2, float delay, ImColor color )
{
	notification_t data( a0, a1, a2, delay, color );
	data.next_notification_padding = notifications.size( ) * 20.0f;
	notifications.push_back( data );
};

void Renderer::RenderNotifications( ) {
	for ( auto& n : notifications ) {
		n.draw( );

		if ( n.dead( ) ) {
			notifications.erase( notifications.begin( ) );
			for ( auto& b : notifications )
				b.next_notification_padding -= 20.0f;
		};
	};
}
int Renderer::NotificationSize( )
{
	return notifications.size( );
}
void Renderer::PushClip( const ImVec2 position, const ImVec2 size )
{
	g_pDrawList->PushClipRect( position, { position.x + size.x, position.y + size.y } );
};

void Renderer::PopClip( )
{
	g_pDrawList->PopClipRect( );
};
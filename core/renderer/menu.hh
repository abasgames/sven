#pragma once
/// <summary>
/// MUI - Created by guwi.
/// Purpose: make something that's not too complicated for the new plebs. -
/// Besides that, i don't really think it's necessary to have the whole "ImGui" menu, when only doing combos, sliders and checkboxes.
/// </summary>
namespace Mui {
	void RenderOldMenu( );
	void RenderNewMenu( );
	void Render( );
	void Setup( );
	extern bool Keys[ 256 ];
	extern bool KeysClicked[ 256 ];
	extern bool KeysUp[ 256 ];

	bool is_key_toggle( int k );
	bool is_key_down( int k );

	void set_key_down( int k );
	void set_key_toggle( int k );

	void reset_key_down( int k );
	void reset_key_toggle( int k );

	bool is_menu_present( );
};
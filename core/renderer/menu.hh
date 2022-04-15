#pragma once
/// <summary>
/// MUI - Created by guwi.
/// Purpose: make something that's not too complicated for the new plebs. -
/// Besides that, i don't really think it's necessary to have the whole "ImGui" menu, when only doing combos, sliders and checkboxes.
/// </summary>
namespace Mui {
	void Render( );
	void Setup( );
	extern bool Keys[ 256 ];
	extern bool KeysClicked[ 256 ];
};
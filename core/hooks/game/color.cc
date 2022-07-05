#include <windows.h>
#include <renderer/menu.hh>
#include <hooks/hooks.hh>
#include <utils/hooking.hh>
#include <utils/utils.hh>
#include <hacks/features.hh>
#include <interfaces/interfaces.hh>

xth::fn_chat_color org_chat_color = nullptr;

void* xth::hk_chat_color( int player_index ) {
	/*
	* code is directly from IDA.
	*/

	/*
	void *result; // eax
    if ( a1 <= 0 )
    {
      result = &unk_10162920;
      if ( a1 )
        result = &unk_10162908;
    }
    else
    {
      switch ( g_PlayerExtraInfo_word_1060630A[0x2C * a1] )
      {
        case 1:
          result = &unk_1016292C;               
          break;
        case 2:
          result = &unk_10162938;
          break;
        case 3:
          result = &unk_10162944;
          break;
        case 4:
          result = &unk_10162950;
          break;
        default:
          result = &unk_10162914;
          break;
      }
    }
    return result;*/
	return org_chat_color( player_index );
};

void xth::color::hook( ) {

    // GetClientColor
	const auto chat_color = xtu::find( "client.dll", "8B 4C 24 04 85 C9 7E" );

	xth::add( chat_color.as< void* >( ), &hk_chat_color, reinterpret_cast< void** >( &org_chat_color ) );
};
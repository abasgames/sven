#include <windows.h>
#include "features.hh"

bool options::show_menu = false;
int options::show_menu_key = VK_INSERT;
int options::show_menu_tab = 1;
bool options::player_important[ 64 ] = { false };
float options::player_important_hue[ 64 ] = { 0.0f };
bool options::player_glue[ 64 ] = { false };
bool options::player_glow[ 64 ] = { false };
int options::player_lock_index = -1;
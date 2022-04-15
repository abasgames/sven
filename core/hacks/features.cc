#include <windows.h>
#include "features.hh"

bool options::show_menu = false;
int options::show_menu_key = VK_INSERT;
int options::show_menu_tab = 0;

bool options::general::airstuck = true;
int options::general::airstuck_key = static_cast< int >( 'V' );
bool options::general::autogauss = false;
int options::general::autogauss_key = static_cast< int >( 0x04 );
bool options::general::antiafk = false;
int options::general::antiafk_mode = 0;
bool options::general::bhop = true;
bool options::general::autostrafe = true;
int options::general::autostrafe_key = static_cast< int >( 0x20 );
int options::general::autostrafe_mode = 0;
bool options::general::last_inventory = false;
int options::general::last_inventory_key = static_cast< int >( 0x04 );
bool options::general::speedhack = false;
int options::general::speedhack_key = static_cast< int >( 0x05 );
int options::general::speedhack_mode = 0;
bool options::general::jumpbug = true;
bool options::general::slowwalk = false;
int options::general::slowwalk_key = static_cast< int >( 0x10 );

bool options::antiaim::enabled = false;
int options::antiaim::antiaim_yaw = 0;
int options::antiaim::antiaim_pitch = 0;
bool options::antiaim::clamp = true;

bool options::fakelag::enabled = false;
int options::fakelag::fakelag_mode = 0;
int options::fakelag::fakelag_key = static_cast< int >( 'B' );

// == // == // == //

bool options::visuals::enabled = true;
bool options::visuals::names = true;
bool options::visuals::healthbar = true;
bool options::visuals::weapons = false;
bool options::visuals::box = true;
int	 options::visuals::box_mode = 0;
bool options::visuals::skeleton = false;
bool options::visuals::chams = false;
bool options::visuals::xqz = false;
bool options::visuals::glow = false;
bool options::visuals::dlight = false;
bool options::visuals::brightness = false;
bool options::visuals::debug = false;

bool options::mobs::enabled = true;
bool options::mobs::names = true;
bool options::mobs::box = true;
int	 options::mobs::box_mode = 0;
bool options::mobs::skeleton = false;
bool options::mobs::chams = false;
bool options::mobs::xqz = false;
bool options::mobs::glow = false;
bool options::mobs::dlight = false;

bool options::recorder::enabled = false;
int	options::recorder::record_record = static_cast< int >( 0x09 );
int	options::recorder::record_play = static_cast< int >( 0x14 );
bool options::recorder::record_clear = false;
#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

enum global_states {
  lightsaber_idle,
  lightsaber_on,
  lightsaber_config
};

enum lightsaber_on_states {
  lightsaber_on_hum,
  lightsaber_on_ignition,
  lightsaber_on_retraction,
  lightsaber_on_bladelockup,
  lightsaber_on_tipmelt,
  lightsaber_on_blasterdeflect,
  lightsaber_on_clash,
  lightsaber_on_swing,
  lightsaber_on_idle,
  lightsaber_on_boot
};

enum config_states {
  config_idle,
  config_soundfont,
  config_volume,
  config_swingsensitivity,
  config_maincolor,
  config_clashcolor,
  config_blastcolor,
  config_batteryLevel,
  config_lastMember
};

//Colors from https://www.reddit.com/r/lightsabers/comments/w5m02g/xenopixel_rgb_color_codes/
enum lightsaberColor {
  Silver_blue = 0x6464C8,
  White = 0x969696,     //(having the values at 150 instead of 255 saves some power and is still bright)
  Pink_red = 0xFF0505,  //(aiming for ANH Vader colour)
  Red = 0xFF0000,
  Blood_Orange = 0xFF0F00,
  Orange = 0xFF1E00,
  Gold = 0xFFB300,
  Yellow = 0xFFFF00,  //(can appear slightly green depending on setup - see more below)
  Neon_Green = 0xB3FF00,
  Lime = 0x46FF00,
  Green = 0x00FF00,
  Mint_Green = 0x00FF3C,
  Cyan = 0x00FF8C,
  Sky_Blue = 0x008CFF,
  Blue = 0x0000FF,
  Purple = 0x7300FF,
  Magenta = 0xDC00FF,
  NumColors = 17
};

#endif
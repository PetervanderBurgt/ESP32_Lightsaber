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
  Silver_blue,
  White,     //(having the values at 150 instead of 255 saves some power and is still bright)
  Pink_red,  //(aiming for ANH Vader colour)
  Red,
  Blood_Orange,
  Orange,
  Gold,
  Yellow,  //(can appear slightly green depending on setup - see more below)
  Neon_Green,
  Lime,
  Green,
  Mint_Green,
  Cyan,
  Sky_Blue,
  Blue,
  Purple,
  Magenta,
  Rainbow,
  UserColor1,
  UserColor2,
  UserColor3,
  NumColors
};


#endif
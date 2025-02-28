#ifndef CONFIGMENU_H
#define CONFIGMENU_H

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

class ConfigMenu {
private:
  // Code for button objects
  config_states current_config_state;


public:
  // Code for button objects
  ConfigMenu();
  void nextConfigMenu();
  void prevConfigMenu();
  void runConfigMenu();

  // Code for helper functions in tasks
};


#endif
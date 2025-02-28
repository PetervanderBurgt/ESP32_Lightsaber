#include <Esp.h>
#include <OneButton.h>
#include "configMenu.h"
#include "globalVariables.h"

extern config_states config_state;

ConfigMenu::ConfigMenu(){
}

void ConfigMenu::runConfigMenu() {
  switch (config_state) {
    case (config_soundfont):
      break;
    case (config_volume):
      break;
    case (config_swingsensitivity):
      break;
    case (config_maincolor):
      break;
    case (config_clashcolor):
      break;
    case (config_blastcolor):
      break;
    case (config_batteryLevel):
      break;
    default:
      // config_idle:
      break;
  }
}

void ConfigMenu::nextConfigMenu() {
  int state = static_cast<int>(config_state) + 1;

  if (state > static_cast<int>(config_lastMember) - 1) {
    config_state = static_cast<config_states>(static_cast<int>(config_idle) + 1);
  } else {
    config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
void ConfigMenu::prevConfigMenu() {
  int state = static_cast<int>(config_state) - 1;
  if (state < 0) {
    config_state = static_cast<config_states>(static_cast<int>(config_lastMember) - 1);
  } else {
    config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
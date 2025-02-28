#include <Esp.h>
#include <OneButton.h>
#include "configMenu.h"
#include "globalVariables.h"

ConfigMenu::ConfigMenu()
  : current_config_state(config_idle) {
}

void ConfigMenu::runConfigMenu() {
  switch (current_config_state) {
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
  int state = static_cast<int>(current_config_state) + 1;

  if (state > static_cast<int>(config_lastMember)) {
    current_config_state = static_cast<config_states>(static_cast<int>(config_idle) + 1);
  } else {
    current_config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
void ConfigMenu::prevConfigMenu() {
  int state = static_cast<int>(current_config_state) - 1;
  if (state < 0) {
    current_config_state = static_cast<config_states>(static_cast<int>(config_lastMember) - 1);
  } else {
    current_config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
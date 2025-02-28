#include <Esp.h>
#include <OneButton.h>
#include "configMenu.h"
#include "globalVariables.h"

extern config_states config_state;

ConfigMenu::ConfigMenu() {
  preferences.begin("Lightsaber", false);       // Lightsaber namespace, and false to be able to read/write
  preferences.getUChar("SoundFont", 1);         // between 1 and 18
  preferences.getUChar("Volume", 30);           // between 0 and 30
  preferences.getUChar("SwingSensitivity", 1);  // between 0 and 255
  preferences.getUInt("MainColor", 0x6464C8);   // Should be a hex color
  preferences.getUInt("ClashColor", 0xFF0505);  // Should be a hex color
  preferences.getUInt("BlastColor", 0xFF0505);  // Should be a hex color
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
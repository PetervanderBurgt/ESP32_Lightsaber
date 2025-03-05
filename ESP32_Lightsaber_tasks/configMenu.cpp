#include <Esp.h>
#include <OneButton.h>
#include "configMenu.h"
#include "globalVariables.h"

extern config_states config_state;
extern uint8_t soundFont;
extern bool configChanged;
extern bool soundFontChanged;

ConfigMenu::ConfigMenu() {
}

void ConfigMenu::readConfig() {
  preferences.begin("Lightsaber", true);             // Lightsaber namespace, and false to be able to read/write
  soundFont = preferences.getUChar("SoundFont", 1);  // between 1 and 18
  Serial.print("Readback Soundfont ");
  Serial.println(soundFont);
  preferences.getUChar("Volume", 30);           // between 0 and 30
  preferences.getUChar("SwingSensitivity", 1);  // between 0 and 255
  preferences.getUInt("MainColor", 0x6464C8);   // Should be a hex color
  preferences.getUInt("ClashColor", 0xFF0505);  // Should be a hex color
  preferences.getUInt("BlastColor", 0xFF0505);  // Should be a hex color
  preferences.end();
}

void ConfigMenu::runConfigMenu(bool mainButtonPressed, bool secondaryButtonPressed) {
  switch (config_state) {
    case (config_soundfont):
      if (mainButtonPressed) {
        soundFontChanged = true;
        soundFont = (soundFont == 18) ? 1 : soundFont + 1;
        Serial.print("increasing soundFont to ");
        Serial.println(soundFont);
      }
      if (secondaryButtonPressed) {
        soundFontChanged = true;
        soundFont = (soundFont == 1) ? 18 : soundFont - 1;
        Serial.print("decreasing soundFont to ");
        Serial.println(soundFont);
      }
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

void ConfigMenu::saveConfigMenu() {
  Serial.print("Saving new Config: ");
  Serial.println(soundFont);
  preferences.begin("Lightsaber", false);        // Lightsaber namespace, and false to be able to read/write
  preferences.putUChar("SoundFont", soundFont);  // between 1 and 18
  // preferences.putUChar("Volume", 30);                // between 0 and 30
  // preferences.putUChar("SwingSensitivity", 1);       // between 0 and 255
  // preferences.putUInt("MainColor", 0x6464C8);        // Should be a hex color
  // preferences.putUInt("ClashColor", 0xFF0505);       // Should be a hex color
  // preferences.putUInt("BlastColor", 0xFF0505);       // Should be a hex color
  preferences.end();
}

void ConfigMenu::nextConfigMenu() {
  configChanged = true;
  int state = static_cast<int>(config_state) + 1;

  if (state > static_cast<int>(config_lastMember) - 1) {
    config_state = static_cast<config_states>(static_cast<int>(config_idle) + 1);
  } else {
    config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
void ConfigMenu::prevConfigMenu() {
  configChanged = true;
  int state = static_cast<int>(config_state) - 1;
  if (state < 0) {
    config_state = static_cast<config_states>(static_cast<int>(config_lastMember) - 1);
  } else {
    config_state = static_cast<config_states>(static_cast<int>(state));
  }
}
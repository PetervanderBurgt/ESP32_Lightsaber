#include <Esp.h>
#include <OneButton.h>
#include "configMenu.h"
#include "globalVariables.h"

extern config_states config_state;
extern uint8_t soundFont;
extern uint8_t dfplayer_volume;
extern uint8_t swingSensitivity;
extern lightsaberColor MainColor;
extern lightsaberColor ClashColor;
extern lightsaberColor BlastColor;

extern bool configChanged;
extern bool soundFontChanged;
extern bool configChangedUp;
extern bool configChangedDown;

extern SemaphoreHandle_t config_mutex;

ConfigMenu::ConfigMenu() {
}

void ConfigMenu::readConfig() {
  preferences.begin("Lightsaber", true);             // Lightsaber namespace, and false to be able to read/write
  soundFont = preferences.getUChar("SoundFont", 1);  // between 1 and 18
  Serial.print("Readback Soundfont ");
  Serial.println(soundFont);
  dfplayer_volume = preferences.getUChar("Volume", 30);              // between 0 and 30
  swingSensitivity = preferences.getUChar("SwingSensitivity", 150);  // between 0 and 255
  preferences.getUInt("MainColor", 0x6464C8);                        // Should be a hex color
  preferences.getUInt("ClashColor", 0xFF0505);                       // Should be a hex color
  preferences.getUInt("BlastColor", 0xFF0505);                       // Should be a hex color
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
      if (mainButtonPressed) {
        configChangedUp = true;
        dfplayer_volume = (dfplayer_volume == 30) ? 0 : dfplayer_volume + 1;
        Serial.print("increasing volume to ");
        Serial.println(dfplayer_volume);
      }
      if (secondaryButtonPressed) {
        configChangedDown = true;
        dfplayer_volume = (dfplayer_volume == 0) ? 30 : dfplayer_volume - 1;
        Serial.print("decreasing volume to ");
        Serial.println(dfplayer_volume);
      }
      break;
    case (config_swingsensitivity):
      if (mainButtonPressed) {
        configChangedUp = true;
        swingSensitivity = (swingSensitivity == 255) ? 0 : swingSensitivity + 1;
        Serial.print("increasing swingSensitivity to ");
        Serial.println(swingSensitivity);
      }
      if (secondaryButtonPressed) {
        configChangedDown = true;
        swingSensitivity = (swingSensitivity == 0) ? 255 : swingSensitivity - 1;
        Serial.print("decreasing swingSensitivity to ");
        Serial.println(swingSensitivity);
      }
      break;
    case (config_maincolor):
      if (mainButtonPressed) {
        configChangedUp = true;
        uint8_t colornumber = static_cast<uint8_t>(MainColor);
        colornumber = (colornumber == (NumColors - 1)) ? 0 : colornumber + 1;
        MainColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed MainColor to ");
        Serial.println(MainColor);
      }
      if (secondaryButtonPressed) {
        configChangedDown = true;
        uint8_t colornumber = static_cast<uint8_t>(MainColor);
        colornumber = (colornumber == 0) ? (NumColors - 1) : colornumber - 1;
        MainColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed MainColor to ");
        Serial.println(MainColor);
      }
      break;
    case (config_clashcolor):
      if (mainButtonPressed) {
        configChangedUp = true;
        uint8_t colornumber = static_cast<uint8_t>(ClashColor);
        colornumber = (colornumber == (NumColors - 1)) ? 0 : colornumber + 1;
        ClashColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed ClashColor to ");
        Serial.println(ClashColor);
      }
      if (secondaryButtonPressed) {
        configChangedDown = true;
        uint8_t colornumber = static_cast<uint8_t>(ClashColor);
        colornumber = (colornumber == 0) ? (NumColors - 1) : colornumber - 1;
        ClashColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed ClashColor to ");
        Serial.println(ClashColor);
      }
      break;
    case (config_blastcolor):
      if (mainButtonPressed) {
        configChangedUp = true;
        uint8_t colornumber = static_cast<uint8_t>(BlastColor);
        colornumber = (colornumber == (NumColors - 1)) ? 0 : colornumber + 1;
        BlastColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed BlastColor to ");
        Serial.println(BlastColor);
      }
      if (secondaryButtonPressed) {
        configChangedDown = true;
        uint8_t colornumber = static_cast<uint8_t>(BlastColor);
        colornumber = (colornumber == 0) ? (NumColors - 1) : colornumber - 1;
        BlastColor = static_cast<lightsaberColor>(colornumber);
        Serial.print("changed BlastColor to ");
        Serial.println(BlastColor);
      }
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
  xSemaphoreTake(config_mutex, portMAX_DELAY);

  configChanged = true;
  uint8_t state = static_cast<uint8_t>(config_state);
  state = (state == (config_lastMember - 1)) ? 1 : state + 1;
  config_state = static_cast<config_states>(state);
  Serial.print("set config_state ");
  Serial.println(config_state);
  xSemaphoreGive(config_mutex);
}
void ConfigMenu::prevConfigMenu() {
  xSemaphoreTake(config_mutex, portMAX_DELAY);
  configChanged = true;
  uint8_t state = static_cast<uint8_t>(config_state);
  state = (state == 1) ? (config_lastMember - 1) : state - 1;
  config_state = static_cast<config_states>(state);
  Serial.print("set config_state ");
  Serial.println(config_state);
  xSemaphoreGive(config_mutex);
}
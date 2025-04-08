#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <Esp.h>
#include "globalVariables.h"
#include <DFMiniMp3.h>

enum lightsaber_sounds {
  sound_boot,
  sound_poweron,
  sound_poweroff,
  sound_swing,
  sound_clash,
  sound_lockup,
  sound_blaster,
  sound_font,
  sound_hum,
  sound_config,
  sound_unknown
};

enum config_sounds {
  config_sound_up,
  config_sound_down,
  config_sound_configmode,
  config_sound_Volume,
  config_sound_Soundfont,
  config_sound_MainColor,
  config_sound_ClashColor,
  config_sound_BlastColor,
  config_sound_max,
  config_sound_min,
  config_sound_diyinolighsaber_boot_t2s,
  config_sound_yes,
  config_sound_no,
  config_sound_diyinojukebox_t2s,
  config_sound_batterynominal,
  config_sound_batterydiminished,
  config_sound_batterylow,
  config_sound_batterycritical,
  config_sound_batteryfull,
  config_sound_bladetype,
  config_sound_ledstringblade,
  config_sound_pixelblade,
  config_sound_starledblade,
  config_sound_ignitionstyle,
  config_sound_flickerStyle,
  config_sound_swingsensitivity,
  config_sound_programmingMode,
  config_sound_storageMediaAccess,
  config_sound_sleepModeInit,
  config_sound_unknown
};

class DFPlayer {
private:
  // Code for sound creating objects
  class Mp3Notify;
  // define a handy type using serial and our notify class
  typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;
  DfMp3 dfmp3;

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void DFPlayerCode();

  // Code for helper functions in tasks
  lightsaber_sounds getEnumFromGlobalTrack(uint16_t globalTrackInt);
  uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt);

  void playStateSound(lightsaber_sounds sound_to_play);
  void playLightsaberTrack(lightsaber_sounds sound_to_play);
  void loopLightsaberTrack(lightsaber_sounds sound_to_play);
  lightsaber_sounds getCurrentLightsaberTrack();

  void playConfigTrack(config_sounds sound_to_play);
  config_sounds getCurrentconfigTrack();

public:
  // Code for sound creating objects
  DFPlayer(HardwareSerial& serialPort);

  // Code for task creation and running
  void startTask();
  void setVolume();

  // Code for helper functions in tasks
  uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont);
};

extern bool dfplayer_ready;

#endif
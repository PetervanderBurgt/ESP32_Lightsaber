#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <Esp.h>
#include "globalVariables.h"

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

  void playLightsaberTrack(lightsaber_sounds sound_to_play);
  void loopLightsaberTrack(lightsaber_sounds sound_to_play);
  lightsaber_sounds getCurrentLightsaberTrack();

public:
  // Code for sound creating objects
  DFPlayer(HardwareSerial& serialPort);

  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
  uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont);
};

extern bool dfplayer_ready;

#endif
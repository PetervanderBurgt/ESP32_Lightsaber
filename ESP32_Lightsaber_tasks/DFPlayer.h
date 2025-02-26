#ifndef DFPLAYER_H
#define DFPLAYER_H

#include <Esp.h>
#include "globalVariables.h"

class DFPlayer {
private:
  class Mp3Notify;
  lightsaber_sounds getEnumFromGlobalTrack(uint16_t globalTrackInt);

  // define a handy type using serial and our notify class
  typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;
  static void runTask(void* pvParameters);
  void DFPlayerCode();
  DfMp3 dfmp3;
  
  uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt);


public:
  DFPlayer(HardwareSerial& serialPort);
  void startTask();
  uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont);
};

extern bool dfplayer_ready;

#endif
#ifndef DFPLAYER_H
#define DFPLAYER_H

#include "globalVariables.h"

class Mp3Notify;

void DFPlayerCode(void* pvParameters);

// define a handy type using serial and our notify class
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;


uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont);
uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt);
lightsaber_sounds getEnumFromGlobalTrack(uint16_t globalTrackInt);

extern bool dfplayer_ready;

#endif
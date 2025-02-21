#ifndef DFPLAYER_H
#define DFPLAYER_H

class Mp3Notify;

void DFPlayerCode(void* pvParameters);

// define a handy type using serial and our notify class
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;

#endif
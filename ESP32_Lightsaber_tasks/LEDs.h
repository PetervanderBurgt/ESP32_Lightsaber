#ifndef LEDS_H
#define LEDS_H
#include <FastLED.h>
#include "globalVariables.h"
#include "pinConfig.h"

class Blade {
private:
  // Code for LED creating objects
  CRGB leds_output_array[NUM_LEDS];

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void LEDCode();

  // Code for helper functions in tasks
  void initLEDS();
  void setLedsWithFlicker(lightsaberColor color);
  void setSolidColor(CRGB color);
  void addBlasterToLeds();
  void setLedsToLockup();
  void setLedsToTipmelt();

public:
  // Code for LED creating objects
  Blade();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool leds_ready;


#endif
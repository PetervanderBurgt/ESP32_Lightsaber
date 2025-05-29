#ifndef LEDS_H
#define LEDS_H
#include <FastLED.h>
#include "globalVariables.h"
#include "pinConfig.h"

class Blade {
private:
  // Code for LED creating objects
  CRGB leds_output_array[NUM_LEDS];
  CRGB crystal_output_array[NUM_LEDS];

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void LEDCode();

  // Code for helper functions in tasks
  void initLEDS();
  void setLedsWithFlicker(lightsaberColor color);
  void setSolidColor(CRGB color);
  void addClashToLeds();
  void addBlasterToLeds();
  void setLedsToLockup();
  void setLedsToTipmelt();
  void setColorOrRainbow(lightsaberColor color);
  void fillRainbowLEDs(CRGB* leds, int count, uint16_t baseHue);
  void fillFlickerLEDs(CRGB* leds, int count, CRGB color, uint16_t& seed, uint16_t speed);

  uint16_t colorNoiseSeed = 0;   // Starting hue for the rainbow animation
  uint16_t colorNoiseSpeed = 2;  // Starting hue for the rainbow animation
  unsigned long clashStartTime = 0;
  const int pulseInterval = 20;  // ms between pulses

public:
  // Code for LED creating objects
  Blade();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool leds_ready;


#endif
#include <Esp.h>
#include <FastLED.h>
#include "buttons.h"
#include "pinConfig.h"
#include "globalVariables.h"

extern lightsaber_on_states lightsaber_on_state;

void LEDCode(void* pvParameters) {
  CRGB leds[NUM_LEDS];
  FastLED.addLeds<WS2811, LED_OUTPUT, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);  // Set brightness level (0-255)

  for (;;) {
    Serial.print("lightsaber_on_state: ");
    Serial.println(lightsaber_on_state);
    switch (lightsaber_on_state) {
      case lightsaber_on_ignition:
        for (int i = 0; i < NUM_LEDS; i++) {
          leds[i] = CRGB(Silver_blue);  // You can change the color here (e.g., CRGB::Blue or CRGB::Red)
          FastLED.show();
          vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to control the speed of the effect (adjust as needed)
        }
        lightsaber_on_state = lightsaber_on_hum;
        break;

      case lightsaber_on_retraction:
        for (int i = NUM_LEDS - 1; i >= 0; i--) {
          leds[i] = CRGB::Black;
          FastLED.show();
          vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        lightsaber_on_state = lightsaber_on_idle;
        break;

      case lightsaber_on_hum:
        fill_solid(leds, NUM_LEDS, CRGB(Silver_blue));  // Hex value 0xFF5733
        FastLED.show();                                 // Update the LEDs to reflect changes
        break;

      default:
        fill_solid(leds, NUM_LEDS, CRGB::Black);  // Hex value 0xFF5733
        FastLED.show();                                 // Update the LEDs to reflect changes
        break;
    }
    // Runs task every 25 MS
    vTaskDelay((1000 / FPS_LEDS) / portTICK_PERIOD_MS);
  }
}
#include <Esp.h>
#include <FastLED.h>
#include "LEDs.h"
#include "pinConfig.h"
#include "globalVariables.h"

bool leds_ready = false;

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;

Blade::Blade() {
  // Any other initialization you need for this class
}

// Start the task by creating a FreeRTOS task
void Blade::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter
  xTaskCreatePinnedToCore(
    runTask,   /* Task function. */
    "LEDTask", /* name of task. */
    2048,      /* Stack size of task */
    this,      /* parameter of the task */
    1,         /* priority of the task */
    NULL,      /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
}

// Static task function called by FreeRTOS
void Blade::runTask(void* pvParameters) {
  // Cast the parameter to a pointer to the MyTaskClass instance
  Blade* instance = static_cast<Blade*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->LEDCode();
}

void Blade::LEDCode() {
  Serial.print("LEDTask running on core ");
  Serial.println(xPortGetCoreID());
  CRGB leds[NUM_LEDS];
  FastLED.addLeds<WS2811, LED_OUTPUT, GRB>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);  // Set brightness level (0-255)

  leds_ready = true;
  for (;;) {
    // Serial.print("lightsaber_on_state: ");
    // Serial.println(lightsaber_on_state);
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
        global_state = lightsaber_idle;
        lightsaber_on_state = lightsaber_on_idle;
        break;

      case lightsaber_on_hum:
        fill_solid(leds, NUM_LEDS, CRGB(Silver_blue));  // Hex value 0xFF5733
        FastLED.show();                                 // Update the LEDs to reflect changes
        break;

      default:
        fill_solid(leds, NUM_LEDS, CRGB::Black);  // Hex value 0xFF5733
        FastLED.show();                           // Update the LEDs to reflect changes
        break;
    }
    // Runs task every 25 MS
    vTaskDelay((1000 / FPS_LEDS) / portTICK_PERIOD_MS);
  }
}
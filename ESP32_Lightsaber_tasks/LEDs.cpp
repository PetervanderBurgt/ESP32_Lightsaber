#include <Esp.h>
#include <FastLED.h>
#include "LEDs.h"
#include "pinConfig.h"
#include "globalVariables.h"

bool leds_ready = false;

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;
extern config_states config_state;

lightsaberColor MainColor = Silver_blue;
lightsaberColor BlastColor = Sky_Blue;
lightsaberColor ClashColor = Pink_red;

uint8_t effectLeds = 0;
uint8_t effectLedsLength = 10;

// This array order should match the one that is given in the enum above
uint32_t lightsaberColorHex[] = {
  0x6464C8,  // Silver_blue,
  0x969696,  // White,     //(having the values at 150 instead of 255 saves some power and is still bright)
  0xFF0505,  // Pink_red,  //(aiming for ANH Vader colour)
  0xFF0000,  // Red,
  0xFF0F00,  // Blood_Orange,
  0xFF1E00,  // Orange,
  0xFFB300,  // Gold,
  0xFFFF00,  // Yellow,  //(can appear slightly green depending on setup - see more below)
  0xB3FF00,  // Neon_Green,
  0x46FF00,  // Lime,
  0x00FF00,  // Green,
  0x00FF3C,  // Mint_Green,
  0x00FF8C,  // Cyan,
  0x008CFF,  // Sky_Blue,
  0x0000FF,  // Blue,
  0x7300FF,  // Purple,
  0xDC00FF,  // Magenta,
  0xABCDEF,  // Rainbow
  0xC0FF00,  // UserColor1,
  0x809BCE,  // UserColor2,
  0xF19953   // UserColor3
};

Blade::Blade() {
  leds_output_array[NUM_LEDS] = {};
  crystal_output_array[NUM_LEDS_CRYSTAL] = {};
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

void Blade::initLEDS() {
  FastLED.addLeds<WS2811, LED_OUTPUT, GRB>(leds_output_array, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2811, CRYSTAL_LED_OUTPUT, GRB>(crystal_output_array, NUM_LEDS_CRYSTAL).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);  // Set brightness level (0-255)
}

void Blade::LEDCode() {
  DEBUG_PRINT("LEDTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / LEDS_HZ));

  initLEDS();

  leds_ready = true;

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    // DEBUG_PRINT("lightsaber_on_state: ");
    // DEBUG_PRINTLN(lightsaber_on_state);
    if (global_state == lightsaber_on) {
      switch (lightsaber_on_state) {
        case lightsaber_on_ignition:
          if (MainColor == Rainbow) {
            fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
          } else {
            fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
          }
          for (int i = 0; i < NUM_LEDS; i++) {
            if (MainColor == Rainbow) {
              // Compute hue based on i (like fill_rainbow does)
              fillRainbowLEDs(leds_output_array, i, colorNoiseSeed);
              colorNoiseSeed += colorNoiseSpeed;  // Animate the noise field over time
            } else {
              fillFlickerLEDs(leds_output_array, i, CRGB(lightsaberColorHex[MainColor]), colorNoiseSeed, colorNoiseSpeed);
            }
            FastLED.show();
            vTaskDelay(10 / portTICK_PERIOD_MS);  // Delay to control the speed of the effect (adjust as needed)
          }
          lightsaber_on_state = lightsaber_on_hum;
          break;

        case lightsaber_on_retraction:
          fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB::Black);
          for (int i = NUM_LEDS - 1; i >= 0; i--) {
            if (MainColor == Rainbow) {
              // Compute hue based on i (like fill_rainbow does)
              fillRainbowLEDs(leds_output_array, i, colorNoiseSeed);
              for (int j = i; j < NUM_LEDS; j++) {
                leds_output_array[j] = CRGB::Black;
              }
              colorNoiseSeed += colorNoiseSpeed;  // Animate the noise field over time
            } else {
              fillFlickerLEDs(leds_output_array, i, CRGB(lightsaberColorHex[MainColor]), colorNoiseSeed, colorNoiseSpeed);
              for (int j = i; j < NUM_LEDS; j++) {
                leds_output_array[j] = CRGB::Black;
              }
            }
            FastLED.show();
            vTaskDelay(10 / portTICK_PERIOD_MS);
          }
          global_state = lightsaber_idle;
          lightsaber_on_state = lightsaber_on_idle;
          break;

        // In the mean time fall through to default hum
        case lightsaber_on_blasterdeflect:
          if (MainColor == Rainbow) {
            fill_rainbow(leds_output_array, NUM_LEDS, colorNoiseSeed, 255 / NUM_LEDS);
            fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
            addBlasterToLeds();
            colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
          } else {
            fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
            setLedsWithFlicker(MainColor);
            addBlasterToLeds();
            DEBUG_PRINTLN("BLASTER LEDS");
          }
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        case lightsaber_on_bladelockup:
          if (MainColor == Rainbow) {
            fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
            setLedsToLockup();
            colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
          } else {
            fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
            setLedsToLockup();
          }
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        case lightsaber_on_tipmelt:
          if (MainColor == Rainbow) {
            fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
            setLedsToTipmelt();
            colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
          } else {
            fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
            setLedsToTipmelt();
          }
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        case lightsaber_on_clash:
          {
            // Add the code for small clashes here
            colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
            if (MainColor == Rainbow) {
              fill_rainbow(leds_output_array, NUM_LEDS, colorNoiseSeed, 255 / NUM_LEDS);
              fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
              addClashToLeds();
            } else {
              setSolidColor(CRGB(lightsaberColorHex[MainColor]));
              addClashToLeds();
            }
            FastLED.show();  // Update the LEDs to reflect changes
          }
          break;

        case lightsaber_on_swing:
        case lightsaber_on_hum:
          if (MainColor == Rainbow) {
            fill_rainbow(leds_output_array, NUM_LEDS, colorNoiseSeed, 255 / NUM_LEDS);
            fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
            colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
          } else {
            setLedsWithFlicker(MainColor);
            fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
          }
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        default:
          setSolidColor(CRGB::Black);
          break;
      }
    } else if (global_state == lightsaber_config) {
      switch (config_state) {
        case config_maincolor:
          setColorOrRainbow(MainColor);
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        case config_clashcolor:
          setColorOrRainbow(ClashColor);
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        case config_blastcolor:
          setColorOrRainbow(BlastColor);
          FastLED.show();  // Update the LEDs to reflect changes
          break;

        default:
          setSolidColor(CRGB::Black);
          break;
      }
    } else {
      // IF idle state, turn on crystal output, but not normal leds
      fill_solid(leds_output_array, NUM_LEDS, CRGB::Black);
      fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, CRGB(lightsaberColorHex[MainColor]));
    }
    // Runs task every 25 MS
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void Blade::setSolidColor(CRGB color) {
  fill_solid(leds_output_array, NUM_LEDS, color);
  fill_solid(crystal_output_array, NUM_LEDS_CRYSTAL, color);
  FastLED.show();
}

void Blade::setLedsWithFlicker(lightsaberColor color) {
  CRGB base = CRGB(lightsaberColorHex[color]);

  for (int i = 0; i < NUM_LEDS; i++) {
    // Start with base color
    leds_output_array[i] = base;
    // Add time and position to get smooth noise
    uint8_t noise = inoise8(i * 10, colorNoiseSeed);  // i*10 gives spacing between LEDs

    // Map noise to a usable flicker brightness range
    uint8_t flicker = map(noise, 0, 255, 108, 255);  // More subtle

    leds_output_array[i].nscale8_video(flicker);  // Safe flicker scaler
    colorNoiseSeed += colorNoiseSpeed;            // Animate the noise field over time
  }
}

void Blade::addClashToLeds() {
  float phase = sin8((colorNoiseSeed * 255) / pulseInterval);  // 0–255 sine wave
  // Add pulsating
  for (int i = 0; i < NUM_LEDS; i++) {
    leds_output_array[i] = blend(leds_output_array[i], CRGB(lightsaberColorHex[ClashColor]), phase);
  }
}

void Blade::addBlasterToLeds() {
  CRGB base = CRGB(lightsaberColorHex[BlastColor]);

  for (int i = effectLeds - effectLedsLength; i < effectLeds; i++) {
    leds_output_array[i] = base;
    // Add time and position to get smooth noise
    uint8_t noise = inoise8(i * 10, colorNoiseSeed);  // i*10 gives spacing between LEDs

    // Map noise to a usable flicker brightness range
    uint8_t flicker = map(noise, 0, 255, 108, 255);  // More subtle

    leds_output_array[i].nscale8_video(flicker);  // Safe flicker scaler
  }
}

void Blade::setLedsToLockup() {
  CRGB main = CRGB(lightsaberColorHex[MainColor]);
  CRGB clash = CRGB(lightsaberColorHex[ClashColor]);

  for (int i = 0; i < NUM_LEDS; i++) {
    int lockupFlick = random(0, 100);
    if (lockupFlick < 80) {
      if (MainColor == Rainbow) {
        leds_output_array[i] = CHSV(colorNoiseSeed + (i * (255 / NUM_LEDS)), 255, 255);
      } else {
        leds_output_array[i] = main;
      }
    } else if (lockupFlick < 90) {
      leds_output_array[i] = clash;
    } else {  // simple white
      leds_output_array[i] = CRGB::White;
    }
    // Add time and position to get smooth noise
    uint8_t noise = inoise8(i * 10, colorNoiseSeed);  // i*10 gives spacing between LEDs

    // Map noise to a usable flicker brightness range
    uint8_t flicker = map(noise, 0, 255, 108, 255);  // More subtle

    leds_output_array[i].nscale8_video(flicker);  // Safe flicker scaler
  }
}

void Blade::setLedsToTipmelt() {
  CRGB main = CRGB(lightsaberColorHex[MainColor]);
  CRGB clash = CRGB(lightsaberColorHex[ClashColor]);

  if (MainColor == Rainbow) {
    fill_rainbow(leds_output_array, NUM_LEDS, colorNoiseSeed, 255 / NUM_LEDS);
  } else {
    setLedsWithFlicker(MainColor);
  }
  for (int i = NUM_LEDS - TIPMELT_LEDS; i < NUM_LEDS; i++) {
    int lockupFlick = random(0, 39);
    if (lockupFlick < 20 && MainColor != Rainbow) {
      leds_output_array[i] = main;
    } else if (lockupFlick < 30) {
      leds_output_array[i] = clash;
    } else {  // simple white
      leds_output_array[i] = CRGB::White;
    }
    // Add time and position to get smooth noise
    uint8_t noise = inoise8(i * 10, colorNoiseSeed);  // i*10 gives spacing between LEDs

    // Map noise to a usable flicker brightness range
    uint8_t flicker = map(noise, 0, 255, 108, 255);  // More subtle

    leds_output_array[i].nscale8_video(flicker);  // Safe flicker scaler
  }
}

void Blade::setColorOrRainbow(lightsaberColor color) {
  if (color == Rainbow) {
    fill_rainbow(leds_output_array, NUM_LEDS, colorNoiseSeed, 255 / NUM_LEDS);
    fill_rainbow(crystal_output_array, NUM_LEDS_CRYSTAL, colorNoiseSeed, 255 / NUM_LEDS_CRYSTAL);
    colorNoiseSeed = colorNoiseSeed + colorNoiseSpeed;
  } else {
    setSolidColor(CRGB(lightsaberColorHex[color]));
  }
}

void Blade::fillRainbowLEDs(CRGB* leds, int count, uint16_t baseHue) {
  for (int j = 0; j < count; j++) {
    uint8_t hue = baseHue + (j * (255 / NUM_LEDS));
    leds[j] = CHSV(hue, 255, 255);
  }
}

void Blade::fillFlickerLEDs(CRGB* leds, int count, CRGB color, uint16_t& seed, uint16_t speed) {
  for (int j = 0; j < count; j++) {
    leds[j] = color;
    uint8_t noise = inoise8(j * 10, seed);
    uint8_t flicker = map(noise, 0, 255, 108, 255);
    leds[j].nscale8_video(flicker);
    seed += speed;
  }
}
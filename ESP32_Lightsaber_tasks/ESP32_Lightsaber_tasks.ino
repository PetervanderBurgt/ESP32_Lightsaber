// this example will play a random track from all on the sd
//
// it expects the sd card to contain some mp3 files

#include <DFMiniMp3.h>
#include <OneButton.h>
#include <FastLED.h>


#define RX_DFPLAYER 16
#define TX_DFPLAYER 17
#define MAIN_BUTTON 21
#define SECOND_BUTTON 22

#define LED_OUTPUT 5
#define NUM_LEDS 100
#define BRIGHTNESS 100
#define FPS_LEDS 120

#define CLICK 200       //200  // ms you need to press a button to be a click
#define LONG_PRESS 400  //400  // ms you need to press a button to be a long press, in action mode

// forward declare the notify class, just the name
//
class Mp3Notify;

// define a handy type using serial and our notify class
//
typedef DFMiniMp3<HardwareSerial, Mp3Notify> DfMp3;

// instance a DfMp3 object,
//
DfMp3 dfmp3(Serial1);

enum global_states {
  lightsaber_idle,
  lightsaber_on,
  lightsaber_config
};

enum lightsaber_on_states {
  lightsaber_on_hum,
  lightsaber_on_ignition,
  lightsaber_on_retraction,
  lightsaber_on_bladelockup,
  lightsaber_on_tipmelt,
  lightsaber_on_blasterdeflect,
  lightsaber_on_clash,
  lightsaber_on_swing,
  lightsaber_on_idle
};

enum lightsaber_sounds {
  sound_boot,
  sound_poweron,
  sound_swing,
  sound_clash,
  sound_lockup,
  sound_blaster,
  sound_font,
  sound_hum
};

enum sound_play {
  sound_play_single,
  sound_play_loop
};

//Colors from https://www.reddit.com/r/lightsabers/comments/w5m02g/xenopixel_rgb_color_codes/
enum lightsaberColor {
  Silver_blue = 0x6464C8,
  White = 0x969696,     //(having the values at 150 instead of 255 saves some power and is still bright)
  Pink_red = 0xFF0505,  //(aiming for ANH Vader colour)
  Red = 0xFF0000,
  Blood_Orange = 0xFF0F00,
  Orange = 0xFF1E00,
  Gold = 0xFFB300,
  Yellow = 0xFFFF00,  //(can appear slightly green depending on setup - see more below)
  Neon_Green = 0xB3FF00,
  Lime = 0x46FF00,
  Green = 0x00FF00,
  Mint_Green = 0x00FF3C,
  Cyan = 0x00FF8C,
  Sky_Blue = 0x008CFF,
  Blue = 0x0000FF,
  Purple = 0x7300FF,
  Magenta = 0xDC00FF,
  NumColors = 17
};

TaskHandle_t DfPlayerTask;
TaskHandle_t SerialTask;
TaskHandle_t ButtonsTask;
TaskHandle_t MPUTask;
TaskHandle_t LEDTask;

uint16_t globalTrackInt = 0;
uint8_t soundFont = 1;
uint8_t global_state = lightsaber_idle;
uint8_t lightsaber_on_state = lightsaber_on_idle;
// Some arduino boards only have one hardware serial port, so a software serial port is needed instead.
// comment out the above definitions and use these
//SoftwareSerial secondarySerial(10, 11); // RX, TX
//typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;
// DfMp3 dfmp3(secondarySerial);

// implement a notification class,
// its member methods will get called
//
class Mp3Notify {
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action) {
    if (source & DfMp3_PlaySources_Sd) {
      Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) {
      Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) {
      Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode) {
    // see DfMp3_Error for code meaning
    Serial.println();
    Serial.print("Communication Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track) {
    Serial.print("Play finished for #");
    Serial.println(track);
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source) {
    PrintlnSourceAction(source, "removed");
  }
};


void setup() {
  xTaskCreatePinnedToCore(
    SerialReadingCode, /* Task function. */
    "SerialReadTask",  /* name of task. */
    2048,              /* Stack size of task */
    NULL,              /* parameter of the task */
    1,                 /* priority of the task */
    &SerialTask,       /* Task handle to keep track of created task */
    1);                /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    ButtonsCode,   /* Task function. */
    "ButtonsTask", /* name of task. */
    2048,          /* Stack size of task */
    NULL,          /* parameter of the task */
    1,             /* priority of the task */
    &ButtonsTask,  /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    MPUCode,   /* Task function. */
    "MPUTask", /* name of task. */
    2048,      /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &MPUTask,  /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    LEDCode,   /* Task function. */
    "LEDTask", /* name of task. */
    2048,      /* Stack size of task */
    NULL,      /* parameter of the task */
    1,         /* priority of the task */
    &LEDTask,  /* Task handle to keep track of created task */
    1);        /* pin task to core 1 */
  vTaskDelay(100);

  xTaskCreatePinnedToCore(
    DFPlayerCode,   /* Task function. */
    "DFPlayerTask", /* name of task. */
    2048,           /* Stack size of task */
    NULL,           /* parameter of the task */
    1,              /* priority of the task */
    &DfPlayerTask,  /* Task handle to keep track of created task */
    1);             /* pin task to core 1 */
  vTaskDelay(100);
}

void loop() {
}

void MPUCode(void* pvParameters) {

  for (;;) {

    // Runs task every 25 MS
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

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

uint16_t fontAndEnumtoTrack(lightsaber_sounds sound, uint8_t soundFont) {
  uint8_t track = 0;
  switch (sound) {
    case sound_boot:
      track = 1;
      break;
    case sound_poweron:
      track = random(2, 5 + 1);
      break;
    case sound_swing:
      track = random(6, 7 + 1);
      break;
    case sound_clash:
      track = random(8, 15 + 1);
      break;
    case sound_lockup:
      track = random(16, 24 + 1);
      break;
    case sound_blaster:
      track = random(25, 28 + 1);
      break;
    case sound_font:
      track = 29;
      break;
    default:
    case sound_hum:
      track = 30;
      break;
  }
  return getGlobalTrackFromFolderandTrack(soundFont + 1, track);
}

uint16_t getGlobalTrackFromFolderandTrack(uint8_t folderInt, uint8_t trackInt) {
  if (folderInt == 1) {
    return trackInt;
  } else {
    return (folderInt - 2) * 30 + 29 + trackInt;
  }
  return 0;
}

void SerialReadingCode(void* pvParameters) {
  Serial.begin(115200);
  Serial.println("initializing...");
  for (;;) {
    if (Serial.available() > 0) {
      // Read the integer from the serial input
      uint8_t enum_number = Serial.readStringUntil('\n').toInt();
      lightsaber_sounds sound_to_play = static_cast<lightsaber_sounds>(enum_number);
      // Print the integer to the serial monitor
      Serial.print("You entered folder: ");
      Serial.println(enum_number);


      globalTrackInt = fontAndEnumtoTrack(sound_to_play, soundFont);
    }
    // Runs task every 25 MS
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}


void DFPlayerCode(void* pvParameters) {
  Serial.print("DFPlayerTask running on core ");
  Serial.println(xPortGetCoreID());
  dfmp3.begin(/*rx =*/RX_DFPLAYER, /*tx =*/TX_DFPLAYER);
  // for boards that support hardware arbitrary pins
  // dfmp3.begin(10, 11); // RX, TX

  // during development, it's a good practice to put the module
  // into a known state by calling reset().
  // You may hear popping when starting and you can remove this
  // call to reset() once your project is finalized
  dfmp3.reset();

  uint16_t version = dfmp3.getSoftwareVersion();
  Serial.print("version ");
  Serial.println(version);

  // show some properties and set the volume
  uint16_t volume = dfmp3.getVolume();
  Serial.print("volume ");
  Serial.println(volume);
  dfmp3.setVolume(30);

  uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
  Serial.print("files ");
  Serial.println(count);

  uint16_t mode = dfmp3.getPlaybackMode();
  Serial.print("playback mode ");
  Serial.println(mode);

  Serial.println("starting...");
  for (;;) {
    if (globalTrackInt != 0) {
      Serial.print("globalTrackInt DF: ");
      Serial.println(globalTrackInt);

      // dfmp3.playGlobalTrack(globalTrackInt);
      dfmp3.loopGlobalTrack(globalTrackInt);
      globalTrackInt = 0;
    }
    dfmp3.loop();
    // Runs task every 25 MS
    vTaskDelay(20 / portTICK_PERIOD_MS);
  }
}

void ButtonsCode(void* pvParameters) {
  OneButton main_button = OneButton(MAIN_BUTTON, true, true);
  main_button.setClickMs(CLICK);
  main_button.setPressMs(LONG_PRESS);
  OneButton second_button = OneButton(SECOND_BUTTON, true, true);
  second_button.setClickMs(CLICK);
  second_button.setPressMs(LONG_PRESS);
  // Handler function for a single click:
  // link the Main Button functions.
  main_button.attachClick(main_button_click);
  main_button.attachDoubleClick(main_button_doubleclick);
  main_button.attachLongPressStart(main_button_longPressStart);
  main_button.attachLongPressStop(main_button_longPressStop);
  main_button.attachDuringLongPress(main_button_longPress);

  // link the Secondary Button functions.
  second_button.attachClick(secondary_button_click);
  second_button.attachDoubleClick(secondary_button_doubleclick);
  second_button.attachLongPressStart(secondary_button_longPressStart);
  second_button.attachLongPressStop(secondary_button_longPressStop);
  second_button.attachDuringLongPress(secondary_button_longPress);

  for (;;) {
    main_button.tick();
    second_button.tick();
    // Runs task every 25 MS
    vTaskDelay(15 / portTICK_PERIOD_MS);
  }
}

// ----- Main Button callback functions
// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void main_button_click() {
  lightsaber_on_state = lightsaber_on_ignition;
  Serial.print("lightsaber_on_state: ");
  Serial.println(lightsaber_on_state);
}
// This function will be called when the button1 was pressed 2 times in a short timeframe.
void main_button_doubleclick() {
  Serial.println("Main Button doubleclick.");
}  // doubleclick1
// This function will be called once, when the button1 is pressed for a long time.
void main_button_longPressStart() {
  Serial.println("Main Button longPress start");
}  // longPressStart1
// This function will be called often, while the button1 is pressed for a long time.
void main_button_longPress() {
  Serial.println("Main Button longPress...");
}  // longPress1
// This function will be called once, when the button1 is released after beeing pressed for a long time.
void main_button_longPressStop() {
  Serial.println("Main Button longPress stop");
}  // longPressStop1

// ... and the same for Secondary Button:
void secondary_button_click() {
  lightsaber_on_state = lightsaber_on_retraction;
  Serial.print("lightsaber_on_state: ");
  Serial.println(lightsaber_on_state);
}  // click2
void secondary_button_doubleclick() {
  Serial.println("Secondary Button doubleclick.");
}  // doubleclick2
void secondary_button_longPressStart() {
  Serial.println("Secondary Button longPress start");
}  // longPressStart2
void secondary_button_longPress() {
  Serial.println("Secondary Button longPress...");
}  // longPress2
void secondary_button_longPressStop() {
  Serial.println("Secondary Button longPress stop");
}  // longPressStop2

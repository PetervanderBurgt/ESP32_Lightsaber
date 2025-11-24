#include <Esp.h>
#include <OneButton.h>
#include "buttons.h"
#include "pinConfig.h"
#include "globalVariables.h"
#include "configMenu.h"
#include "SaberWeb.h"


SaberWeb saberwebConfig = SaberWeb();

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;
extern config_states config_state;
extern bool configStart;
extern ConfigMenu menu;

extern uint8_t effectLeds;
extern uint8_t effectLedsLength;

bool buttons_ready = false;

bool blaster_enabled = false;
bool lockup_enabled = false;

Buttons::Buttons(button_types button_type)
  : current_button_type(button_type) {
  // Any other initialization you need for this class
}

// Start the task by creating a FreeRTOS task
void Buttons::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter

  xTaskCreatePinnedToCore(
    runTask,       /* Task function. */
    "ButtonsTask", /* name of task. */
    BUTTONS_TASK_STACK_SIZE,          /* Stack size of task */
    this,          /* parameter of the task */
    BUTTONS_TASK_PRIORITY,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
}

// Static task function called by FreeRTOS
void Buttons::runTask(void* pvParameters) {
  DEBUG_PRINT("ButtonTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  // Cast the parameter to a pointer to the MyTaskClass instance
  Buttons* instance = static_cast<Buttons*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->ButtonsCode();
}

void Buttons::initButton() {
  if (current_button_type == button_double_main) {
    button.setup(MAIN_BUTTON, INPUT_PULLUP, true);
    button.setClickMs(CLICK);
    button.setPressMs(LONG_PRESS);
    button.attachClick(main_button_click);
    button.attachDoubleClick(main_button_doubleclick);
    button.attachLongPressStart(main_button_longPressStart);
    button.attachLongPressStop(main_button_longPressStop);
    button.attachDuringLongPress(main_button_longPress);

    initRGBPins(MAIN_RED, MAIN_GREEN, MAIN_BLUE);

  } else if (current_button_type == button_double_secondary) {
    button.setup(SECOND_BUTTON, INPUT_PULLUP, true);
    button.setClickMs(CLICK);
    button.setPressMs(LONG_PRESS);
    button.attachClick(secondary_button_click);
    button.attachDoubleClick(secondary_button_doubleclick);
    button.attachLongPressStart(secondary_button_longPressStart);
    button.attachLongPressStop(secondary_button_longPressStop);
    button.attachDuringLongPress(secondary_button_longPress);

    initRGBPins(SECOND_RED, SECOND_GREEN, SECOND_BLUE);
  }
}

void Buttons::initRGBPins(uint8_t rPin, uint8_t gPin, uint8_t bPin) {
  pinMode(rPin, OUTPUT);
  pinMode(gPin, OUTPUT);
  pinMode(bPin, OUTPUT);
  digitalWrite(rPin, HIGH);
  digitalWrite(gPin, HIGH);
  digitalWrite(bPin, HIGH);
}

void Buttons::ButtonsCode() {

  DEBUG_PRINT("Button running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / BUTTONS_HZ));

  initButton();

  buttons_ready = true;

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {

    button.tick();

    // DEBUG_PRINTLN("Button Tick.");

    if (global_state == lightsaber_on) {
      vTaskPrioritySet(NULL, BUTTONS_TASK_PRIORITY);
      //set button green
      setLEDColorForButton(current_button_type, LOW, LOW, HIGH);

    } else if (global_state == lightsaber_config) {
      vTaskPrioritySet(NULL, BUTTONS_TASK_PRIORITY + 1);
      //set button green
      setLEDColorForButton(current_button_type, LOW, HIGH, LOW);

    } else {
      vTaskPrioritySet(NULL, BUTTONS_TASK_PRIORITY);
      if (lightsaber_on_state == lightsaber_on_web_config) {
        //set button green
        setLEDColorForButton(current_button_type, LOW, HIGH, LOW);
      } else {
        //set button red
        setLEDColorForButton(current_button_type, HIGH, LOW, LOW);
      }
    }
    // Runs task every 15 MS
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void Buttons::setLEDColorForButton(button_types btn, bool r, bool g, bool b) {
  if (btn == button_double_main) {
    setLEDColor(MAIN_RED, MAIN_GREEN, MAIN_BLUE, r, g, b);
  } else if (btn == button_double_secondary) {
    setLEDColor(SECOND_RED, SECOND_GREEN, SECOND_BLUE, r, g, b);
  }
}

void Buttons::setLEDColor(uint8_t rPin, uint8_t gPin, uint8_t bPin, bool r, bool g, bool b) {
  digitalWrite(rPin, !r);
  digitalWrite(gPin, !g);
  digitalWrite(bPin, !b);
}

// ----- Main Button callback functions
// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void Buttons::main_button_click() {
  DEBUG_PRINTLN("Main Button click.");
  if (global_state == lightsaber_idle) {
    if (lightsaber_on_state == lightsaber_on_idle) {
      global_state = lightsaber_on;
      DEBUG_PRINT("global_state: ");
      DEBUG_PRINTLN(global_state);
      lightsaber_on_state = lightsaber_on_ignition;
      DEBUG_PRINT("lightsaber_on_state: ");
      DEBUG_PRINTLN(lightsaber_on_state);
    }
  } else if (global_state == lightsaber_config) {
    menu.runConfigMenu(true, false);
  } else if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      blaster_enabled = true;
      DEBUG_PRINTLN("blaster_enabled");
    }
  }
}
// This function will be called when the button1 was pressed 2 times in a short timeframe.
void Buttons::main_button_doubleclick() {
  DEBUG_PRINTLN("Main Button doubleclick.");
  if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      effectLeds = random(effectLedsLength, NUM_LEDS);
      lightsaber_on_state = lightsaber_on_tipmelt;
      vTaskDelay(TIPMELT_FX_DURATION);
      lightsaber_on_state = lightsaber_on_hum;
    }
  } else if (global_state == lightsaber_idle) {
    if (lightsaber_on_state == lightsaber_on_idle) {
      DEBUG_PRINTLN("Saber web task started from Button.");
      saberwebConfig.startTask();
      lightsaber_on_state = lightsaber_on_web_config;
    } else if (lightsaber_on_state == lightsaber_on_web_config) {
      DEBUG_PRINTLN("Saber web task stopped from Button.");
      saberwebConfig.stopTask();
      lightsaber_on_state = lightsaber_on_idle;
    }
  }
}  // doubleclick1
// This function will be called once, when the button1 is pressed for a long time.
void Buttons::main_button_longPressStart() {
  DEBUG_PRINTLN("Main Button longPress start");
  if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      DEBUG_PRINT("global_state: ");
      DEBUG_PRINTLN(global_state);
      lightsaber_on_state = lightsaber_on_retraction;
      DEBUG_PRINT("lightsaber_on_state: ");
      DEBUG_PRINTLN(lightsaber_on_state);
    }
  } else if (global_state == lightsaber_config) {
    menu.nextConfigMenu();
  }
}  // longPressStart1
// This function will be called often, while the button1 is pressed for a long time.
void Buttons::main_button_longPress() {
  DEBUG_PRINTLN("Main Button longPress...");
}  // longPress1
// This function will be called once, when the button1 is released after beeing pressed for a long time.
void Buttons::main_button_longPressStop() {
  DEBUG_PRINTLN("Main Button longPress stop");
}  // longPressStop1

// ... and the same for Secondary Button:
void Buttons::secondary_button_click() {
  DEBUG_PRINTLN("Secondary Button click.");
  if (global_state == lightsaber_config) {
    menu.runConfigMenu(false, true);
  } else if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      effectLeds = random(effectLedsLength, NUM_LEDS);
      lightsaber_on_state = lightsaber_on_blasterdeflect;
      vTaskDelay(BLASTER_FX_DURATION);
      lightsaber_on_state = lightsaber_on_hum;
    }
  }
}  // click2
void Buttons::secondary_button_doubleclick() {
  DEBUG_PRINTLN("Secondary Button doubleclick.");
  if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      lockup_enabled = true;
    }
  }
}  // doubleclick2
void Buttons::secondary_button_longPressStart() {
  if (global_state == lightsaber_idle) {
    global_state = lightsaber_config;
    DEBUG_PRINT("global_state: ");
    DEBUG_PRINTLN(global_state);
    config_state = config_idle;
    DEBUG_PRINT("config_state: ");
    DEBUG_PRINTLN(config_state);
    configStart = true;
  } else if (global_state == lightsaber_config) {
    menu.saveConfigMenu();
    global_state = lightsaber_idle;
    DEBUG_PRINT("global_state: ");
    DEBUG_PRINTLN(global_state);
    lightsaber_on_state = lightsaber_on_boot;
    DEBUG_PRINT("lightsaber_on_state: ");
    DEBUG_PRINTLN(lightsaber_on_state);
    config_state = config_idle;
    DEBUG_PRINT("config_state: ");
    DEBUG_PRINTLN(config_state);
    configStart = true;
  } else if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      lightsaber_on_state = lightsaber_on_bladelockup;
    }
  }
  DEBUG_PRINTLN("Secondary Button longPress start");
}  // longPressStart2
void Buttons::secondary_button_longPress() {
  DEBUG_PRINTLN("Secondary Button longPress...");
}  // longPress2
void Buttons::secondary_button_longPressStop() {
  if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_bladelockup) {
      lightsaber_on_state = lightsaber_on_hum;
    }
  }
  DEBUG_PRINTLN("Secondary Button longPress stop");
}  // longPressStop2
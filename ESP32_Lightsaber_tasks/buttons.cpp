#include <Esp.h>
#include <OneButton.h>
#include "buttons.h"
#include "pinConfig.h"
#include "globalVariables.h"
#include "configMenu.h"

extern global_states global_state;
extern lightsaber_on_states lightsaber_on_state;
extern config_states config_state;
extern bool configStart;

bool buttons_ready = false;

ConfigMenu menu;

Buttons::Buttons(button_types button_type)
  : current_button_type(button_type) {
}

// Start the task by creating a FreeRTOS task
void Buttons::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter

  xTaskCreatePinnedToCore(
    runTask,       /* Task function. */
    "ButtonsTask", /* name of task. */
    2048,          /* Stack size of task */
    this,          /* parameter of the task */
    1,             /* priority of the task */
    NULL,          /* Task handle to keep track of created task */
    1);            /* pin task to core 1 */
}

// Static task function called by FreeRTOS
void Buttons::runTask(void* pvParameters) {
  Serial.print("ButtonTask running on core ");
  Serial.println(xPortGetCoreID());
  // Cast the parameter to a pointer to the MyTaskClass instance
  Buttons* instance = static_cast<Buttons*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->ButtonsCode();
}

void Buttons::ButtonsCode() {

  Serial.print("Button running on core ");
  Serial.println(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / BUTTONS_HZ));

  OneButton button;
  menu = ConfigMenu();
  if (current_button_type == button_double_main) {
    button.setup(MAIN_BUTTON, INPUT_PULLUP, true);
    button.setClickMs(CLICK);
    button.setPressMs(LONG_PRESS);
    button.attachClick(main_button_click);
    button.attachDoubleClick(main_button_doubleclick);
    button.attachLongPressStart(main_button_longPressStart);
    button.attachLongPressStop(main_button_longPressStop);
    button.attachDuringLongPress(main_button_longPress);

  } else if (current_button_type == button_double_secondary) {
    button.setup(SECOND_BUTTON, INPUT_PULLUP, true);
    button.setClickMs(CLICK);
    button.setPressMs(LONG_PRESS);
    button.attachClick(secondary_button_click);
    button.attachDoubleClick(secondary_button_doubleclick);
    button.attachLongPressStart(secondary_button_longPressStart);
    button.attachLongPressStop(secondary_button_longPressStop);
    button.attachDuringLongPress(secondary_button_longPress);
  }

  buttons_ready = true;

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    button.tick();
    // Runs task every 15 MS
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

// ----- Main Button callback functions
// This function will be called when the button1 was pressed 1 time (and no 2. button press followed).
void Buttons::main_button_click() {
  Serial.println("Main Button click.");
  if (global_state == lightsaber_idle) {
    if (lightsaber_on_state == lightsaber_on_idle) {
      global_state = lightsaber_on;
      Serial.print("global_state: ");
      Serial.println(global_state);
      lightsaber_on_state = lightsaber_on_ignition;
      Serial.print("lightsaber_on_state: ");
      Serial.println(lightsaber_on_state);
    }
  }
}
// This function will be called when the button1 was pressed 2 times in a short timeframe.
void Buttons::main_button_doubleclick() {
  Serial.println("Main Button doubleclick.");
}  // doubleclick1
// This function will be called once, when the button1 is pressed for a long time.
void Buttons::main_button_longPressStart() {
  Serial.println("Secondary Button longPress start");
  if (global_state == lightsaber_on) {
    if (lightsaber_on_state == lightsaber_on_hum) {
      Serial.print("global_state: ");
      Serial.println(global_state);
      lightsaber_on_state = lightsaber_on_retraction;
      Serial.print("lightsaber_on_state: ");
      Serial.println(lightsaber_on_state);
    }
  } else if (global_state == lightsaber_config) {
    menu.nextConfigMenu();
  }
}  // longPressStart1
// This function will be called often, while the button1 is pressed for a long time.
void Buttons::main_button_longPress() {
  Serial.println("Main Button longPress...");
}  // longPress1
// This function will be called once, when the button1 is released after beeing pressed for a long time.
void Buttons::main_button_longPressStop() {
  Serial.println("Main Button longPress stop");
}  // longPressStop1

// ... and the same for Secondary Button:
void Buttons::secondary_button_click() {
  Serial.println("Secondary Button click.");
}  // click2
void Buttons::secondary_button_doubleclick() {
  Serial.println("Secondary Button doubleclick.");
}  // doubleclick2
void Buttons::secondary_button_longPressStart() {
  if (global_state == lightsaber_idle) {
    global_state = lightsaber_config;
    Serial.print("global_state: ");
    Serial.println(global_state);
    config_state = config_idle;
    Serial.print("config_state: ");
    Serial.println(config_state);
    configStart = true;
  } else if (global_state == lightsaber_config) {
    global_state = lightsaber_idle;
    Serial.print("global_state: ");
    Serial.println(global_state);
    lightsaber_on_state = lightsaber_on_boot;
    Serial.print("lightsaber_on_state: ");
    Serial.println(lightsaber_on_state);
    config_state = config_idle;
    Serial.print("config_state: ");
    Serial.println(config_state);
    configStart = true;
  }
  Serial.println("Secondary Button longPress start");
}  // longPressStart2
void Buttons::secondary_button_longPress() {
  Serial.println("Secondary Button longPress...");
}  // longPress2
void Buttons::secondary_button_longPressStop() {
  Serial.println("Secondary Button longPress stop");
}  // longPressStop2
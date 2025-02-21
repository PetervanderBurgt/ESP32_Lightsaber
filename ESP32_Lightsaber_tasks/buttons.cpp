#include <Esp.h>
#include <OneButton.h>
#include "buttons.h"
#include "pinConfig.h"
#include "globalVariables.h"

OneButton main_button(MAIN_BUTTON, true, true);
OneButton second_button(SECOND_BUTTON, true, true);
extern lightsaber_on_states lightsaber_on_state;

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

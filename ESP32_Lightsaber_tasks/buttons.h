#ifndef BUTTON_H
#define BUTTON_H
#include <OneButton.h>

#define CLICK 200       //200  // ms you need to press a button to be a click
#define LONG_PRESS 400  //400  // ms you need to press a button to be a long press, in action mode

enum button_types {
  button_single,
  button_double_main,
  button_double_secondary
};

class Buttons {
private:
  // Code for button objects
  button_types current_button_type;
  OneButton button;
  // Code for task creation and running
  static void runTask(void* pvParameters);
  void ButtonsCode();

  // Code for helper functions in tasks
  void initButton();
  void initRGBPins(uint8_t rPin, uint8_t gPin, uint8_t bPin);

  void setLEDColorForButton(button_types btn, bool r, bool g, bool b);
  void setLEDColor(uint8_t rPin, uint8_t gPin, uint8_t bPin, bool r, bool g, bool b);

  static void main_button_click();
  static void main_button_doubleclick();
  static void main_button_longPressStart();
  static void main_button_longPress();
  static void main_button_longPressStop();

  static void secondary_button_click();
  static void secondary_button_doubleclick();
  static void secondary_button_longPressStart();
  static void secondary_button_longPress();
  static void secondary_button_longPressStop();

public:
  // Code for button objects
  Buttons(button_types button_type);
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};




extern bool buttons_ready;


#endif
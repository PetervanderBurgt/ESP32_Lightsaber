#ifndef BUTTON_H
#define BUTTON_H

#define CLICK 200       //200  // ms you need to press a button to be a click
#define LONG_PRESS 400  //400  // ms you need to press a button to be a long press, in action mode

void ButtonsCode(void* pvParameters);

void main_button_click();
void main_button_doubleclick();
void main_button_longPressStart();
void main_button_longPress();
void main_button_longPressStop();

void secondary_button_click();
void secondary_button_doubleclick();
void secondary_button_longPressStart();
void secondary_button_longPress();
void secondary_button_longPressStop();

extern bool buttons_ready;


#endif
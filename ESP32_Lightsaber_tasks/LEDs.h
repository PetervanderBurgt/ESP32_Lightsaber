#ifndef LEDS_H
#define LEDS_H


class Blade {
private:
  // Code for LED creating objects

  // Code for task creation and running
  static void runTask(void* pvParameters);
  void LEDCode();

  // Code for helper functions in tasks

public:
  // Code for LED creating objects
  Blade();
  // Code for task creation and running
  void startTask();

  // Code for helper functions in tasks
};

extern bool leds_ready;


#endif
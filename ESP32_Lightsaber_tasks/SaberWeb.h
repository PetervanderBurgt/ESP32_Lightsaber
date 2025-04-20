#ifndef SABERWEB_H
#define SABERWEB_H

#include <Esp.h>
#include <Preferences.h>
#include "globalVariables.h"
#include "pinConfig.h"
#include "ESPAsyncWebServer.h"



class SaberWeb {
private:
  // Code for button objects
  Preferences preferences;
  // Code for task creation and running
  static void runTask(void* pvParameters);
  void WEBCode();

  void readConfig();
  void saveSaberWeb(AsyncWebServerRequest *request);
  void initSaberWeb();
  void runSaberWeb();
  static SaberWeb* instance;  // static pointer to store 'this'
  TaskHandle_t saberWebTaskHandle = NULL;

public:
  // Code for button objects
  SaberWeb();
  // Code for task creation and running
  void startTask();
  void stopTask();


  // Code for helper functions in tasks
};


#endif
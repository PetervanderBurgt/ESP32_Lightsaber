#ifndef CONFIGMENU_H
#define CONFIGMENU_H

#include <Esp.h>
#include <Preferences.h>
#include "globalVariables.h"



class ConfigMenu {
private:
  // Code for button objects
  Preferences preferences;


public:
  // Code for button objects
  ConfigMenu();
  void nextConfigMenu();
  void prevConfigMenu();
  void runConfigMenu();


  // Code for helper functions in tasks
};


#endif
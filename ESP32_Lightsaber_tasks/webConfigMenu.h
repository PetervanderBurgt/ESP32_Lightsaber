#ifndef CONFIGMENU_H
#define CONFIGMENU_H

#include <Esp.h>
#include <Preferences.h>
#include "globalVariables.h"
#include "pinConfig.h"



class WebConfigMenu {
private:
  // Code for button objects
  Preferences preferences;


public:
  // Code for button objects
  ConfigMenu();
  void readConfig();
  void saveWebConfigMenu();
  void runWebConfigMenu(bool mainButtonPressed, bool secondaryButtonPressed);


  // Code for helper functions in tasks
};


#endif
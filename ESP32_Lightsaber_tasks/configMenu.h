#ifndef CONFIGMENU_H
#define CONFIGMENU_H

#include <Esp.h>
#include <Preferences.h>
#include "globalVariables.h"
#include "pinConfig.h"



class ConfigMenu {
private:
  // Code for button objects
  Preferences preferences;
  lightsaberColor cycleColor(lightsaberColor color, bool increase);

public:
  // Code for button objects
  ConfigMenu();
  void readConfig();
  void nextConfigMenu();
  void prevConfigMenu();
  void saveConfigMenu();
  void runConfigMenu(bool mainButtonPressed, bool secondaryButtonPressed);


  // Code for helper functions in tasks
};


#endif
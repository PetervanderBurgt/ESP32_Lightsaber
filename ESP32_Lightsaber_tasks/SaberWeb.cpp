#include <Esp.h>
#include <OneButton.h>
#include "SaberWeb.h"
#include "globalVariables.h"
#include "pinConfig.h"
#include "DFPlayer.h"

#include <DNSServer.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include "ESPAsyncWebServer.h"

const char* ssid = "SaberWeb";
const char* password = "lightsaber123";

AsyncWebServer server(80);
SaberWeb* SaberWeb::instance = nullptr;
// Set up DNS server (this will redirect any requests to the AP's IP address)
DNSServer dnsServer;
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Lightsaber Config</title>
</head>
<body>
  <h2>Configure Your Lightsaber</h2>
  <form action="/submit" method="GET">

    <label>Sound Font:</label>
    <select name="SoundFont">
        <option value="1">Vanilla Lyte</option>
        <option value="2">Vanilla Dark</option>
        <option value="3">Vanilla Swirl</option>
        <option value="4">In Vader</option>
        <option value="5">Fallen Son</option>
        <option value="6">Ancient Weapon</option>
        <option value="7">Arcane</option>
        <option value="8">Assassin</option>
        <option value="9">Astrium Blue</option>
        <option value="10">Corvus Tano</option>
        <option value="11">Crispity</option>
        <option value="12">Decimate</option>
        <option value="13">Exalted</option>
        <option value="14">Grey</option>
        <option value="15">Metropolis</option>
        <option value="16">Ra</option>
        <option value="17">Senate Majority</option>
        <option value="18">Slave</option>
    </select><br><br>

    <label for="Volume">Volume:</label><br>
    <input type="range" id="Volume" name="Volume" min="1" max="30" value="30" oninput="this.nextElementSibling.value = this.value">
    <output>30</output>
    <br><br>

    <label for="SwingSensitivity">Swing sensitivity:</label><br>
    <input type="range" id="SwingSensitivity" name="SwingSensitivity" min="0" max="16000" step="160" value="960" oninput="this.nextElementSibling.value = this.value">
    <output>960</output>
    <br><br>

    <label>Main Color:</label>
    <select id="mainColor" name="mainColor"  onchange="updateDropdownColor(this)">
        <option value="0" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="1" style="background-color:#969696; color:white;">White</option>
        <option value="2" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="3" style="background-color:#FF0000; color:white;">Red</option>
        <option value="4" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="5" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="6" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="7" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="8" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="9" style="background-color:#46FF00; color:black;">Lime</option>
        <option value="10" style="background-color:#00FF00; color:black;">Green</option>
        <option value="11" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="12" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="13" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="14" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="15" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="16" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="17" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="18" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="19" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="20" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>Clash Color:</label>
    <select id="clashColor" name="clashColor"  onchange="updateDropdownColor(this)">
        <option value="0" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="1" style="background-color:#969696; color:white;">White</option>
        <option value="2" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="3" style="background-color:#FF0000; color:white;">Red</option>
        <option value="4" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="5" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="6" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="7" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="8" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="9"  style="background-color:#46FF00; color:black;">Lime</option>
        <option value="10" style="background-color:#00FF00; color:black;">Green</option>
        <option value="11" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="12" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="13" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="14" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="15" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="16" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="17" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="18" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="19" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="20" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>Blast Color:</label>
    <select id="blastColor" name="blastColor"  onchange="updateDropdownColor(this)">
        <option value="0" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="1" style="background-color:#969696; color:white;">White</option>
        <option value="2" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="3" style="background-color:#FF0000; color:white;">Red</option>
        <option value="4" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="5" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="6" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="7" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="8" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="9"  style="background-color:#46FF00; color:black;">Lime</option>
        <option value="10" style="background-color:#00FF00; color:black;">Green</option>
        <option value="11" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="12" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="13" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="14" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="15" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="16" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="17" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="18" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="19" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="20" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>User Color 1:</label>
    <input type="color" name="UserColor1" value="#C0FF00">
    </br>Or enter Hex Color:
    <input type="text" name="UserColor1Hex" value="" pattern="^([A-Fa-f0-9]{6})$">
    <br><br>

    <label>User Color 2:</label>
    <input type="color" name="UserColor2" value="#809BCE">
    </br>Or enter Hex Color:
    <input type="text" name="UserColor2Hex" value="" pattern="^([A-Fa-f0-9]{6})$">
    <br><br>
    
    <label>User Color 3:</label>
    <input type="color" name="UserColor3" value="#F19953">
    </br>Or enter Hex Color:
    <input type="text" name="UserColor3Hex" value="" pattern="^([A-Fa-f0-9]{6})$">
    <br><br>

    <input type="submit" value="Save Settings">
  </form>
</body>
</html>
  )rawliteral";

class CaptivePortalHandler : public AsyncWebHandler {
public:
  CaptivePortalHandler() {}
  virtual ~CaptivePortalHandler() {}

  bool canHandle(AsyncWebServerRequest* request) {
    return request->url() == "/";
  }

  void handleRequest(AsyncWebServerRequest* request) {
    if (request->method() == HTTP_GET && request->url() == "/") {
      request->send_P(200, "text/html", index_html);
    } else {
      request->send_P(200, "text/html", index_html);
    }
  }
};

extern config_states config_state;
extern uint8_t soundFont;
extern uint8_t dfplayer_volume;
extern uint16_t swingSensitivity;
extern lightsaberColor MainColor;
extern lightsaberColor ClashColor;
extern lightsaberColor BlastColor;
extern uint32_t lightsaberColorHex[];

extern bool configChanged;
extern bool soundFontChanged;
extern bool configChangedUp;
extern bool configChangedDown;

extern DFPlayer audio;

extern SemaphoreHandle_t config_mutex;

SaberWeb::SaberWeb() {
  saberWebTaskHandle = NULL;
  SaberWeb::instance = this;
}

// Start the task by creating a FreeRTOS task
void SaberWeb::startTask() {
  // Create the task, passing `this` (the instance of the class) as the parameter
  DEBUG_PRINTLN("Saber web task started.");
  xTaskCreatePinnedToCore(
    runTask,             /* Task function. */
    "WEBTask",           /* name of task. */
    8192,                /* Stack size of task */
    this,                /* parameter of the task */
    1,                   /* priority of the task */
    &saberWebTaskHandle, /* Task handle to keep track of created task */
    0);                  /* pin task to core 0 */
}

void SaberWeb::stopTask() {
  if (saberWebTaskHandle != NULL) {
    vTaskDelete(saberWebTaskHandle);
    saberWebTaskHandle = NULL;
    DEBUG_PRINTLN("Saber web task stopped.");
  }
}

// Static task function called by FreeRTOS
void SaberWeb::runTask(void* pvParameters) {
  // Cast the parameter to a pointer to the MyTaskClass instance
  SaberWeb* instance = static_cast<SaberWeb*>(pvParameters);

  // Call the instance's non-static method (the actual task)
  instance->WEBCode();
}

void SaberWeb::WEBCode() {
  DEBUG_PRINT("LEDTask running on core ");
  DEBUG_PRINTLN(xPortGetCoreID());
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS((1000 / 60));

  initSaberWeb();

  xLastWakeTime = xTaskGetTickCount();
  for (;;) {
    runSaberWeb();
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void SaberWeb::initSaberWeb() {
  if (WiFi.softAP(ssid, password)) {
    DEBUG_PRINTLN("Access Point Started");
    DEBUG_PRINT("IP Address: ");
    DEBUG_PRINTLN(WiFi.softAPIP());  // Shows the IP address
  }
  // Set up DNS server to redirect all HTTP requests to the AP IP
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Handle the root URL and redirect to a specific page
  server.onNotFound([&](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  });

  server.on("/submit", [](AsyncWebServerRequest* request) {
    instance->saveSaberWeb(request);
    request->send(200, "text/html", "<h3>Settings saved!</h3><a href='/'>Back</a>");
  });
  server.addHandler(new CaptivePortalHandler()).setFilter(ON_AP_FILTER);  //only when requested from AP

  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.setTTL(300);
  dnsServer.start(53, "*", WiFi.softAPIP());

  server.begin();
}

void SaberWeb::runSaberWeb() {
  dnsServer.processNextRequest();
}

// Handle the submitted form
void SaberWeb::saveSaberWeb(AsyncWebServerRequest* request) {
  String str_soundFont = request->getParam("SoundFont")->value();
  String str_volume = request->getParam("Volume")->value();
  String str_swingSensitivity = request->getParam("SwingSensitivity")->value();
  String str_mainColor = request->getParam("mainColor")->value();
  String str_blastColor = request->getParam("blastColor")->value();
  String str_clashColor = request->getParam("clashColor")->value();
  String str_customColor1 = (request->getParam("UserColor1")->value()).substring(1);
  String str_customColor1Hex = (request->getParam("UserColor1Hex")->value());
  String str_customColor2 = (request->getParam("UserColor2")->value()).substring(1);
  String str_customColor2Hex = (request->getParam("UserColor2Hex")->value());
  String str_customColor3 = (request->getParam("UserColor3")->value()).substring(1);
  String str_customColor3Hex = (request->getParam("UserColor3Hex")->value());

  uint8_t web_soundFont = str_soundFont.toInt();
  uint8_t web_volume = str_volume.toInt();
  uint16_t web_swingSensitivity = str_swingSensitivity.toInt();
  uint8_t web_mainColor = str_mainColor.toInt();
  uint8_t web_blastColor = str_blastColor.toInt();
  uint8_t web_clashColor = str_clashColor.toInt();
  uint32_t web_customColor1 = (uint32_t)strtol(str_customColor1.c_str(), NULL, 16);
  uint32_t web_customColor1Hex = (uint32_t)strtol(str_customColor1Hex.c_str(), NULL, 16);
  uint32_t web_customColor2 = (uint32_t)strtol(str_customColor2.c_str(), NULL, 16);
  uint32_t web_customColor2Hex = (uint32_t)strtol(str_customColor2Hex.c_str(), NULL, 16);
  uint32_t web_customColor3 = (uint32_t)strtol(str_customColor3.c_str(), NULL, 16);
  uint32_t web_customColor3Hex = (uint32_t)strtol(str_customColor3Hex.c_str(), NULL, 16);

  if (str_customColor1Hex.length() > 0) {
    web_customColor1 = web_customColor1Hex;
  }
  if (str_customColor2Hex.length() > 0) {
    web_customColor2 = web_customColor2Hex;
  }
  if (str_customColor3Hex.length() > 0) {
    web_customColor3 = web_customColor3Hex;
  }

  DEBUG_PRINTLN("=== Config Received ===");
  DEBUG_PRINT("soundFont: ");
  DEBUG_PRINTLN(web_soundFont);
  DEBUG_PRINT("volume: ");
  DEBUG_PRINTLN(web_volume);
  DEBUG_PRINT("swingSensitivity: ");
  DEBUG_PRINTLN(web_swingSensitivity);
  DEBUG_PRINT("mainColor: ");
  DEBUG_PRINTLN(web_mainColor);
  DEBUG_PRINT("blastColor: ");
  DEBUG_PRINTLN(web_blastColor);
  DEBUG_PRINT("clashColor: ");
  DEBUG_PRINTLN(web_clashColor);
  DEBUG_PRINT("customColor1: ");
  DEBUG_PRINTLN(web_customColor1);
  DEBUG_PRINT("customColor1Hex: ");
  DEBUG_PRINTLN(web_customColor1Hex);
  DEBUG_PRINT("customColor2: ");
  DEBUG_PRINTLN(web_customColor2);
  DEBUG_PRINT("customColor2Hex: ");
  DEBUG_PRINTLN(web_customColor2Hex);
  DEBUG_PRINT("customColor3: ");
  DEBUG_PRINTLN(web_customColor3);
  DEBUG_PRINT("customColor3Hex: ");
  DEBUG_PRINTLN(web_customColor3Hex);

  soundFont = web_soundFont;  // between 1 and 18
  dfplayer_volume = web_volume;                               // between 0 and 30
  swingSensitivity = web_swingSensitivity;                  // between 0 and 16000
  MainColor = static_cast<lightsaberColor>(web_mainColor);     // Should be a enum number (0-20)
  ClashColor = static_cast<lightsaberColor>(web_clashColor);   // Should be a enum number (0-20)
  BlastColor = static_cast<lightsaberColor>(web_blastColor);  // Should be a enum number (0-20)
  lightsaberColorHex[18] = web_customColor1;               // Should be a uint32 hex color
  lightsaberColorHex[19] = web_customColor2;               // Should be a uint32 hex color
  lightsaberColorHex[20] = web_customColor3;               // Should be a uint32 hex color

  DEBUG_PRINTLN("Saving new Config");
  preferences.begin("Lightsaber", false);                                // Lightsaber namespace, and false to be able to read/write
  preferences.putUChar("SoundFont", soundFont);                          // between 1 and 18
  preferences.putUChar("Volume", dfplayer_volume);                       // between 0 and 30
  preferences.putUShort("SwingSensitivity", swingSensitivity);           // between 0 and 16000
  preferences.putUChar("MainColor", static_cast<uint8_t>(MainColor));    // Should be a enum number (0-20)
  preferences.putUChar("ClashColor", static_cast<uint8_t>(ClashColor));  // Should be a enum number (0-20)
  preferences.putUChar("BlastColor", static_cast<uint8_t>(BlastColor));  // Should be a enum number (0-20)
  preferences.putUInt("UserColor1", web_customColor1);  // Should be a uint32 hex color
  preferences.putUInt("UserColor2", web_customColor2);  // Should be a uint32 hex color
  preferences.putUInt("UserColor3", web_customColor3);  // Should be a uint32 hex color
  preferences.end();

  audio.setVolume();  // set the new volume on dfplayer
}

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

const char* ssid = "Saber_Config";
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
    <select name="Sound Font">
        <option value="Vanilla Lyte">Vanilla Lyte</option>
        <option value="Vanilla Dark">Vanilla Dark</option>
        <option value="Vanilla Swirl">Vanilla Swirl</option>
        <option value="In Vader">In Vader</option>
        <option value="Fallen Son">Fallen Son</option>
        <option value="Ancient Weapon">Ancient Weapon</option>
        <option value="Arcane">Arcane</option>
        <option value="Assassin">Assassin</option>
        <option value="Astrium Blue">Astrium Blue</option>
        <option value="Corvus Tano">Corvus Tano</option>
        <option value="Crispity">Crispity</option>
        <option value="Decimate">Decimate</option>
        <option value="Exalted">Exalted</option>
        <option value="Grey">Grey</option>
        <option value="Metropolis">Metropolis</option>
        <option value="Ra">Ra</option>
        <option value="Senate Majority">Senate Majority</option>
        <option value="Slave">Slave</option>
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
        <option value="0x6464C8" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="0x969696" style="background-color:#969696; color:white;">White</option>
        <option value="0xFF0505" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="0xFF0000" style="background-color:#FF0000; color:white;">Red</option>
        <option value="0xFF0F00" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="0xFF1E00" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="0xFFB300" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="0xFFFF00" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="0xB3FF00" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="0x46FF00" style="background-color:#46FF00; color:black;">Lime</option>
        <option value="0x00FF00" style="background-color:#00FF00; color:black;">Green</option>
        <option value="0x00FF3C" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="0x00FF8C" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="0x008CFF" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="0x0000FF" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="0x7300FF" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="0xDC00FF" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="0xABCDEF" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="0xC0FF00" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="0x809BCE" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="0xF19953" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>Clash Color:</label>
    <select id="clashColor" name="clashColor"  onchange="updateDropdownColor(this)">
        <option value="0x6464C8" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="0x969696" style="background-color:#969696; color:white;">White</option>
        <option value="0xFF0505" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="0xFF0000" style="background-color:#FF0000; color:white;">Red</option>
        <option value="0xFF0F00" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="0xFF1E00" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="0xFFB300" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="0xFFFF00" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="0xB3FF00" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="0x46FF00" style="background-color:#46FF00; color:black;">Lime</option>
        <option value="0x00FF00" style="background-color:#00FF00; color:black;">Green</option>
        <option value="0x00FF3C" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="0x00FF8C" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="0x008CFF" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="0x0000FF" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="0x7300FF" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="0xDC00FF" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="0xABCDEF" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="0xC0FF00" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="0x809BCE" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="0xF19953" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>Blast Color:</label>
    <select id="blastColor" name="blastColor"  onchange="updateDropdownColor(this)">
        <option value="0x6464C8" style="background-color:#6464C8; color:white;">Silver_blue</option>
        <option value="0x969696" style="background-color:#969696; color:white;">White</option>
        <option value="0xFF0505" style="background-color:#FF0505; color:white;">Pink_red</option>
        <option value="0xFF0000" style="background-color:#FF0000; color:white;">Red</option>
        <option value="0xFF0F00" style="background-color:#FF0F00; color:white;">Blood_Orange</option>
        <option value="0xFF1E00" style="background-color:#FF1E00; color:white;">Orange</option>
        <option value="0xFFB300" style="background-color:#FFB300; color:black;">Gold</option>
        <option value="0xFFFF00" style="background-color:#FFFF00; color:black;">Yellow</option>
        <option value="0xB3FF00" style="background-color:#B3FF00; color:black;">Neon_Green</option>
        <option value="0x46FF00" style="background-color:#46FF00; color:black;">Lime</option>
        <option value="0x00FF00" style="background-color:#00FF00; color:black;">Green</option>
        <option value="0x00FF3C" style="background-color:#00FF3C; color:black;">Mint_Green</option>
        <option value="0x00FF8C" style="background-color:#00FF8C; color:black;">Cyan</option>
        <option value="0x008CFF" style="background-color:#008CFF; color:black;">Sky_Blue</option>
        <option value="0x0000FF" style="background-color:#0000FF; color:white;">Blue</option>
        <option value="0x7300FF" style="background-color:#7300FF; color:white;">Purple</option>
        <option value="0xDC00FF" style="background-color:#DC00FF; color:white;">Magenta</option>
        <option value="0xABCDEF" style="background-color:#ABCDEF; color:black;">Rainbow</option>
        <option value="0xC0FF00" style="background-color:#C0FF00; color:black;">UserColor1</option>
        <option value="0x809BCE" style="background-color:#809BCE; color:black;">UserColor2</option>
        <option value="0xF19953" style="background-color:#F19953; color:black;">UserColor3</option>
    </select><br><br>

    <label>User Color 1:</label>
    <input type="color" name="UserColor1" value="#C0FF00"><br><br>

    <label>User Color 2:</label>
    <input type="color" name="UserColor2" value="#809BCE"><br><br>

    <label>User Color 3:</label>
    <input type="color" name="UserColor3" value="#F19953"><br><br>

    <input type="submit" value="Save Settings">
  </form>
</body>
</html>
  )rawliteral";

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest* request) {
    //request->addInterestingHeader("ANY");
    return true;
  }

  void handleRequest(AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
  }
};

extern config_states config_state;
extern uint8_t soundFont;
extern uint8_t dfplayer_volume;
extern uint16_t swingSensitivity;
extern lightsaberColor MainColor;
extern lightsaberColor ClashColor;
extern lightsaberColor BlastColor;

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
    Serial.println("Access Point Started");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());  // Shows the IP address
  }
  // Set up DNS server to redirect all HTTP requests to the AP IP
  dnsServer.start(53, "*", WiFi.softAPIP());

  // Handle the root URL and redirect to a specific page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send_P(200, "text/html", index_html);
    Serial.println("Client Connected");
  });

  server.on("/submit", [](AsyncWebServerRequest* request) {
    instance->saveSaberWeb(request);
    request->send(200, "text/html", "<h3>Settings saved!</h3><a href='/'>Back</a>");
  });
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);  //only when requested from AP

  server.begin();
}

void SaberWeb::runSaberWeb() {
  dnsServer.processNextRequest();
}

// Handle the submitted form
void SaberWeb::saveSaberWeb(AsyncWebServerRequest* request) {
  String soundFont = request->getParam("Sound Font")->value();
  String volume = request->getParam("Volume")->value();
  String swingSensitivity = request->getParam("SwingSensitivity")->value();
  String mainColor = request->getParam("mainColor")->value();
  String blastColor = request->getParam("blastColor")->value();
  String clashColor = request->getParam("clashColor")->value();
  String customColor1 = request->getParam("UserColor1")->value();
  String customColor2 = request->getParam("UserColor2")->value();
  String customColor3 = request->getParam("UserColor3")->value();

  Serial.println("=== Config Received ===");
  Serial.println("soundFont: " + soundFont);
  Serial.println("volume: " + volume);
  Serial.println("swingSensitivity: " + swingSensitivity);
  Serial.println("mainColor: " + mainColor);
  Serial.println("blastColor: " + blastColor);
  Serial.println("clashColor: " + clashColor);
  Serial.println("customColor1: " + customColor1);
  Serial.println("customColor2: " + customColor2);
  Serial.println("customColor3: " + customColor3);

  // // Optionally parse hex color
  // uint32_t hex = strtol(customColor.c_str() + 1, NULL, 16);
  // uint8_t r = (hex >> 16) & 0xFF;
  // uint8_t g = (hex >> 8) & 0xFF;
  // uint8_t b = hex & 0xFF;

  // Serial.printf("Parsed RGB: (%d, %d, %d)\n", r, g, b);
}

#include "config_manager.h"
#include "keyboard_handler.h"
#include "slack_notifier.h"
#include "web_server_handler.h"
#include "security_manager.h"

#include "Adafruit_TinyUSB.h"
#include <ArduinoOTA.h>
#include <WiFi.h>

//#define DEBUG

void setup() {
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  // Initialize keyboard
  initializeKeyboard();

  // If already enumerated, additional class driver begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.begin(115200);
  
  #ifdef DEBUG
    //Add delay for IDE to connect to Serial
    delay(2000);
    Serial.println("Loading configuration...");
  #endif
  
  // Load configuration
  if (!loadConfiguration()) {
    Serial.println("Failed to load configuration! Rebooting...");
    delay(1000);
    rp2040.restart();
  }
  
  #ifdef DEBUG
    Serial.println("Configuration loaded successfully.");
    printConfiguration();
  #endif

  // Connect to WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(getConfigValue("ssid").c_str(), getConfigValue("password").c_str());
  
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    rp2040.restart();
  }
  
  ArduinoOTA.begin();

  // Initialize built-in LED
  #ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif

  // Initialize web server
  initializeWebServer();

  String pageName = getConfigValue("pagename");
  String page = pageName.isEmpty() ? "/" : "/" + pageName;
  Serial.printf("Open http://%s%s\n", WiFi.localIP().toString().c_str(), page.c_str());
}

void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
    // Manual call tud_task since it isn't called by Core's background
    TinyUSBDevice.task();
  #endif

  ArduinoOTA.handle();
  handleWebServerClient();

  // Toggle LED every second
  #ifdef LED_BUILTIN
    static unsigned long lastToggle = 0;
    static bool ledState = false;

    if (millis() - lastToggle >= 1000) {
      lastToggle = millis();
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);
    }
  #endif
}
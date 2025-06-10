#include "Adafruit_TinyUSB.h"
#include <ArduinoOTA.h>
#include <LEAmDNS.h>
#include <LittleFS.h>
#include <WebServer.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <map>
#include <vector>

//#define DEBUG

typedef struct {
  uint8_t modifier;
  uint8_t keycode;
} HidKey;

// config values from config.txt
std::map<String, String> config;

// Blocklist to store failed Auth attempts and blocked IPs and block timestamps
std::map<String, int> failedAttempts;
std::map<String, unsigned long> blockedIPs;

#define ALLOWED_ATTEMPTS 3
#define BLOCK_DURATION 60 * 60 * 1000 //60 minutes

std::map<String, String> loadConfig() {
  std::map<String, String> config;

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return config;
  }

  File configFile = LittleFS.open("/config.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open config.txt");
    return config;
  }

  while (configFile.available()) {
    String line = configFile.readStringUntil('\n');
    int sep = line.indexOf('=');
    if (sep > 0) {
      String key = line.substring(0, sep);
      String val = line.substring(sep + 1);
      val.trim();
      config[key] = val;
    }
  }

  configFile.close();
  return config;
}

// HID report descriptor using TinyUSB's template
// Single Report (no ID) descriptor
uint8_t const desc_hid_report[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

WebServer server(80);

const char* www_realm = "Custom Auth Realm";
String authFailResponse = "Authentication Failed";

// USB HID object. For ESP32 these values cannot be changed after this declaration
// desc report, desc len, protocol, interval, use out endpoint
Adafruit_USBD_HID usb_hid;

std::map<String, uint8_t> MODIFIER_KEYS = {
  {"SHIFT",  KEYBOARD_MODIFIER_LEFTSHIFT},
  {"LSHIFT", KEYBOARD_MODIFIER_LEFTSHIFT},
  {"RSHIFT", KEYBOARD_MODIFIER_RIGHTSHIFT},

  {"CTRL",   KEYBOARD_MODIFIER_LEFTCTRL},
  {"LCTRL",  KEYBOARD_MODIFIER_LEFTCTRL},
  {"RCTRL",  KEYBOARD_MODIFIER_RIGHTCTRL},

  {"ALT",    KEYBOARD_MODIFIER_LEFTALT},
  {"LALT",   KEYBOARD_MODIFIER_LEFTALT},
  {"RALT",   KEYBOARD_MODIFIER_RIGHTALT},

  {"GUI",    KEYBOARD_MODIFIER_LEFTGUI},
  {"WIN",    KEYBOARD_MODIFIER_LEFTGUI},
  {"LWIN",   KEYBOARD_MODIFIER_LEFTGUI},
  {"RWIN",   KEYBOARD_MODIFIER_RIGHTGUI},
  {"CMD",    KEYBOARD_MODIFIER_LEFTGUI},
  {"LCMD",   KEYBOARD_MODIFIER_LEFTGUI},
  {"RCMD",   KEYBOARD_MODIFIER_RIGHTGUI}
};


std::map<String, HidKey> SPECIAL_KEYS = {
  {"ESC",        {0, HID_KEY_ESCAPE}},
  {"F1",         {0, HID_KEY_F1}},
  {"F2",         {0, HID_KEY_F2}},
  {"F3",         {0, HID_KEY_F3}},
  {"F4",         {0, HID_KEY_F4}},
  {"F5",         {0, HID_KEY_F5}},
  {"F6",         {0, HID_KEY_F6}},
  {"F7",         {0, HID_KEY_F7}},
  {"F8",         {0, HID_KEY_F8}},
  {"F9",         {0, HID_KEY_F9}},
  {"F10",        {0, HID_KEY_F10}},
  {"F11",        {0, HID_KEY_F11}},
  {"F12",        {0, HID_KEY_F12}},
  {"F13",        {0, HID_KEY_F13}},
  {"F14",        {0, HID_KEY_F14}},
  {"F15",        {0, HID_KEY_F15}},
  {"F16",        {0, HID_KEY_F16}},
  {"F17",        {0, HID_KEY_F17}},
  {"F18",        {0, HID_KEY_F18}},
  {"F19",        {0, HID_KEY_F19}},
  {"F20",        {0, HID_KEY_F20}},
  {"F21",        {0, HID_KEY_F21}},
  {"F22",        {0, HID_KEY_F22}},
  {"F23",        {0, HID_KEY_F23}},
  {"F24",        {0, HID_KEY_F24}},

  {"BACKSPACE",  {0, HID_KEY_BACKSPACE}},
  {"DEL",        {0, HID_KEY_DELETE}},
  {"DELETE",     {0, HID_KEY_DELETE}},

  {"TAB",        {0, HID_KEY_TAB}},
  {"ENTER",      {0, HID_KEY_ENTER}},

  {"SHIFT",      {0, HID_KEY_SHIFT_LEFT}},
  {"LSHIFT",     {0, HID_KEY_SHIFT_LEFT}},
  {"RSHIFT",     {0, HID_KEY_SHIFT_RIGHT}},

  {"CTRL",       {0, HID_KEY_CONTROL_LEFT}},
  {"LCTRL",      {0, HID_KEY_CONTROL_LEFT}},
  {"RCTRL",      {0, HID_KEY_CONTROL_RIGHT}},

  {"ALT",        {0, HID_KEY_ALT_LEFT}},
  {"LALT",       {0, HID_KEY_ALT_LEFT}},
  {"RALT",       {0, HID_KEY_ALT_RIGHT}},

  {"GUI",        {0, HID_KEY_GUI_LEFT}},
  {"WIN",        {0, HID_KEY_GUI_LEFT}},
  {"LWIN",       {0, HID_KEY_GUI_LEFT}},
  {"RWIN",       {0, HID_KEY_GUI_RIGHT}},
  {"CMD",        {0, HID_KEY_GUI_LEFT}},
  {"LCMD",       {0, HID_KEY_GUI_LEFT}},
  {"RCMD",       {0, HID_KEY_GUI_RIGHT}},

  {"SPACE",      {0, HID_KEY_SPACE}},
  {"PLUS",       {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_EQUAL}}, // SHIFT + '=' is '+'

  {"PRTSCRN",    {0, HID_KEY_PRINT_SCREEN}},
  {"SCRLLOCK",   {0, HID_KEY_SCROLL_LOCK}},
  {"PAUSE",      {0, HID_KEY_PAUSE}},

  {"INSERT",     {0, HID_KEY_INSERT}},
  {"HOME",       {0, HID_KEY_HOME}},
  {"END",        {0, HID_KEY_END}},
  {"PAGEUP",     {0, HID_KEY_PAGE_UP}},
  {"PAGEDOWN",   {0, HID_KEY_PAGE_DOWN}},

  {"UP",         {0, HID_KEY_ARROW_UP}},
  {"DOWN",       {0, HID_KEY_ARROW_DOWN}},
  {"LEFT",       {0, HID_KEY_ARROW_LEFT}},
  {"RIGHT",      {0, HID_KEY_ARROW_RIGHT}},

  {"KP1",        {0, HID_KEY_KEYPAD_1}},
  {"KP2",        {0, HID_KEY_KEYPAD_2}},
  {"KP3",        {0, HID_KEY_KEYPAD_3}},
  {"KP4",        {0, HID_KEY_KEYPAD_4}},
  {"KP5",        {0, HID_KEY_KEYPAD_5}},
  {"KP6",        {0, HID_KEY_KEYPAD_6}},
  {"KP7",        {0, HID_KEY_KEYPAD_7}},
  {"KP8",        {0, HID_KEY_KEYPAD_8}},
  {"KP9",        {0, HID_KEY_KEYPAD_9}},
  {"KP0",        {0, HID_KEY_KEYPAD_0}},
  {"KPDIV",      {0, HID_KEY_KEYPAD_DIVIDE}},
  {"KPMUL",      {0, HID_KEY_KEYPAD_MULTIPLY}},
  {"KPSUB",      {0, HID_KEY_KEYPAD_SUBTRACT}},
  {"KPADD",      {0, HID_KEY_KEYPAD_ADD}},
  {"KPENTER",    {0, HID_KEY_KEYPAD_ENTER}},
  {"KP.",        {0, HID_KEY_KEYPAD_DECIMAL}},
  {"KP=",        {0, HID_KEY_KEYPAD_EQUAL}},
  {"KP,",        {0, HID_KEY_KEYPAD_COMMA}}
};

// Converts ASCII character to HID keycode + modifier (e.g., Shift)
HidKey asciiToHid(char c) {
  HidKey result = {0, 0};

  if (c >= 'a' && c <= 'z') {
    result.keycode = HID_KEY_A + (c - 'a');
    result.modifier = 0;
  } else if (c >= 'A' && c <= 'Z') {
    result.keycode = HID_KEY_A + (c - 'A');
    result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  } else if (c >= '1' && c <= '9') {
    result.keycode = HID_KEY_1 + (c - '1');
    result.modifier = 0;
  } else if (c == '0') {
    result.keycode = HID_KEY_0;
    result.modifier = 0;
  }

  // Symbols and punctuation
  else {
    switch (c) {
      // Shifted punctuation
      case '!': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_1}; break;
      case '@': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_2}; break;
      case '#': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_3}; break;
      case '$': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_4}; break;
      case '%': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_5}; break;
      case '^': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_6}; break;
      case '&': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_7}; break;
      case '*': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_8}; break;
      case '(': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_9}; break;
      case ')': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_0}; break;
      case '-': result.keycode = HID_KEY_MINUS; break;
      case '_': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_MINUS}; break;
      case '=': result.keycode = HID_KEY_EQUAL; break;
      case '+': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_EQUAL}; break;
      case '[': result.keycode = HID_KEY_BRACKET_LEFT; break;
      case '{': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_BRACKET_LEFT}; break;
      case ']': result.keycode = HID_KEY_BRACKET_RIGHT; break;
      case '}': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_BRACKET_RIGHT}; break;
      case '\\': result.keycode = HID_KEY_BACKSLASH; break;
      case '|': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_BACKSLASH}; break;
      case ';': result.keycode = HID_KEY_SEMICOLON; break;
      case ':': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_SEMICOLON}; break;
      case '\'': result.keycode = HID_KEY_APOSTROPHE; break;
      case '"': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_APOSTROPHE}; break;
      case '`': result.keycode = HID_KEY_GRAVE; break;
      case '~': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_GRAVE}; break;
      case ',': result.keycode = HID_KEY_COMMA; break;
      case '<': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_COMMA}; break;
      case '.': result.keycode = HID_KEY_PERIOD; break;
      case '>': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_PERIOD}; break;
      case '/': result.keycode = HID_KEY_SLASH; break;
      case '?': result = {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_SLASH}; break;
    }
  }
  return result;
}

// Send a string via USB keyboard
void sendStringAsKeystrokes(const String& input) {
  uint8_t const report_id = 0;

  // Split input by spaces
  int start = 0;
  while (start < input.length()) {
    int end = input.indexOf(' ', start);
    if (end == -1) end = input.length();
    String segment = input.substring(start, end);
    segment.trim();

    if (segment.length() == 0) {
      start = end + 1;
      continue;
    }

    if (segment.indexOf('+') != -1) {
      // Handle chorded input like CTRL+ALT+DEL
      uint8_t modifier = 0;
      uint8_t keycode[6] = {0};
      uint8_t keyIndex = 0;

      int chordStart = 0;
      while (chordStart < segment.length()) {
        int chordEnd = segment.indexOf('+', chordStart);
        if (chordEnd == -1) chordEnd = segment.length();
        String key = segment.substring(chordStart, chordEnd);
        key.trim();

        // First check if it's a modifier key
        if (MODIFIER_KEYS.count(key)) {
          modifier |= MODIFIER_KEYS[key];
        } else {
          HidKey hk = {0, 0};
          if (SPECIAL_KEYS.count(key)) {
            hk = SPECIAL_KEYS[key];
          } else if (key.length() == 1) {
            hk = asciiToHid(key.charAt(0));  // Fall back to ASCII mapping
          }

          if (hk.keycode != 0 && keyIndex < 6) {
            keycode[keyIndex++] = hk.keycode;
            modifier |= hk.modifier;
          }
        }

        chordStart = chordEnd + 1;
      }
      while (!usb_hid.ready()) delay(1);
      usb_hid.keyboardReport(report_id, modifier, keycode);
      #ifdef DEBUG
        Serial.printf("modifier '%d', keycodes: ", modifier);
        for (int i = 0; i < 6; ++i) {
          Serial.printf("%d ", keycode[i]);
        }
        Serial.println();
      #endif
      delay(5);
      usb_hid.keyboardRelease(report_id);
      delay(5);

    } else {
      // Handle special key or multiple characters
      String key = segment;
      key.trim();

      if (SPECIAL_KEYS.count(key)) {
        // Special key (e.g. ENTER, CTRL, etc.)
        HidKey hk = SPECIAL_KEYS[key];

        uint8_t keycode[6] = {hk.keycode};
        while (!usb_hid.ready()) delay(1);
        usb_hid.keyboardReport(report_id, hk.modifier, keycode);
        #ifdef DEBUG
          Serial.printf("modifier '%d', keycode '%d'\n", hk.modifier, hk.keycode);
        #endif
        delay(5);
        usb_hid.keyboardRelease(report_id);
        delay(5);
      } else {
        // Treat as a sequence of characters
        for (size_t i = 0; i < key.length(); ++i) {
          HidKey hk = asciiToHid(key.charAt(i));
          if (hk.keycode == 0) continue; // skip unsupported chars

          uint8_t keycode[6] = {hk.keycode};
          while (!usb_hid.ready()) delay(1);
          usb_hid.keyboardReport(report_id, hk.modifier, keycode);
          #ifdef DEBUG
            Serial.printf("Char '%c' -> modifier '%d', keycode '%d'\n", key.charAt(i), hk.modifier, hk.keycode);
          #endif
          delay(5);
          usb_hid.keyboardRelease(report_id);
          delay(5);
        }
      }
    }

    start = end + 1;
  }
}

void sendSlackMsg(const String& msg, const char* slack_path)
{
  const char* slack_host = "hooks.slack.com";
  const int slack_port = 443;

  WiFiClientSecure client;

  client.setInsecure(); // Skip SSL cert verification

  // Connect to Slack
  if (!client.connect(slack_host, slack_port)) {
    #ifdef DEBUG
      Serial.println("Connection to Slack failed");
    #endif
    return;
  }
  // Compose Slack message
  String payload = "{\"text\": \"" + msg + "\"}";

  // Send POST request
    client.printf("POST %s HTTP/1.1\r\n", slack_path);
    client.printf("Host: %s\r\n", slack_host);
    client.println("Content-Type: application/json");
    client.printf("Content-Length: %d\r\n", payload.length());
    client.println("Connection: close\r\n");
    client.println(payload);

  // Read and print response
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
    #ifdef DEBUG
      Serial.println(line);
    #endif
  }

  #ifdef DEBUG
    String response = client.readString();
    Serial.println("Slack response:");
    Serial.println(response);
  #endif
}

// the setup function runs once when you press reset or power the board
void setup() {
  // Manual begin() is required on core without built-in support e.g. mbed rp2040
  if (!TinyUSBDevice.isInitialized()) {
    TinyUSBDevice.begin(0);
  }

  // Setup HID
  usb_hid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usb_hid.setPollInterval(2);
  usb_hid.setReportDescriptor(desc_hid_report, sizeof(desc_hid_report));
  usb_hid.setStringDescriptor("TinyUSB Keyboard");

  usb_hid.begin();

  // If already enumerated, additional class driverr begin() e.g msc, hid, midi won't take effect until re-enumeration
  if (TinyUSBDevice.mounted()) {
    TinyUSBDevice.detach();
    delay(10);
    TinyUSBDevice.attach();
  }

  Serial.begin(115200);
  #ifdef DEBUG
    Serial.println("Loading Credentials...");
  #endif
  config = loadConfig();
  
  #ifdef DEBUG
    //Add a delay to enable Serial Monitor to connect
    delay(2000);
    Serial.println("Credentials loaded.");
    for (const auto& pair : config) {
      Serial.print("Key: '");
      Serial.print(pair.first);
      Serial.print("' => Value: '");
      Serial.print(pair.second);
      Serial.println("'");
    }    
  #endif
  WiFi.mode(WIFI_STA);
  WiFi.begin(config["ssid"].c_str(), config["password"].c_str());
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("WiFi Connect Failed! Rebooting...");
    delay(1000);
    rp2040.restart();
  }
  ArduinoOTA.begin();

  // led pin
  #ifdef LED_BUILTIN
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
  #endif

  String page = config.count("pagename") ? "/" + config["pagename"] : "/";
  String slack_webhook = config.count("slack_webhook") ? config["slack_webhook"] : "";
  server.on(page, [page, slack_webhook]() {
    String clientIP = server.client().remoteIP().toString();
    unsigned long now = millis();

    if (blockedIPs.count(clientIP) && now - blockedIPs[clientIP] < BLOCK_DURATION) {
      server.send(403, "text/plain", "Blocked.");
      return;
    }
    else {
      if (slack_webhook != "") {
        sendSlackMsg("Client IP: " + clientIP + " successfully authenticated to page " + page + ".", config["slack_webhook"].c_str());
      }
      blockedIPs.erase(clientIP);
    }

    if (!server.authenticate(config["username"].c_str(), config["userpass"].c_str())) {
      if (slack_webhook != "") {
        sendSlackMsg("Client IP: " + clientIP + " failed auth on page " + page + ".", config["slack_webhook"].c_str());
      }
      failedAttempts[clientIP] = failedAttempts.count(clientIP) ? failedAttempts[clientIP] + 1 : 0;

      if (failedAttempts[clientIP] >= 3) {
        if (slack_webhook != "") {
          sendSlackMsg("Client IP: " + clientIP + " blocked on page " + page + ".", config["slack_webhook"].c_str());
        }
        blockedIPs[clientIP] = now;
        failedAttempts.erase(clientIP);  // Reset count after blocking
        server.send(403, "text/plain", "Too many failed attempts - Blocked");
        return;
      }
      server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      return;
    }
  
    // Successful auth, reset fail count for this IP
    failedAttempts.erase(clientIP);

    server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head><title>USB Keyboard</title></head>
<body>
  <h2>Send Keystrokes via USB</h2>
  <form action="/send" method="POST">
    <input type="text" name="keystroke" placeholder="Enter string" style="width:70%;">
    <input type="submit" value="Send">
  </form>
</body>
</html>
    )rawliteral");
  });


  server.on("/send", HTTP_POST, [slack_webhook]() {
    if (!server.authenticate(config["username"].c_str(), config["userpass"].c_str())) {
      server.send(403, "text/plain", "Too many failed attempts - you are blocked");
    }
    String data = server.arg("keystroke");
    sendStringAsKeystrokes(data);
    server.send(200, "text/plain", "Sent: " + data);
    String clientIP = server.client().remoteIP().toString();
    if (slack_webhook != "") {
      sendSlackMsg("Client IP: " + clientIP + " sent text: '" + data + "'", config["slack_webhook"].c_str());
    }
  });

  server.begin();
  #ifdef DEBUG
    Serial.printf("Open http://%s%s\n",WiFi.localIP().toString().c_str(), page.c_str());
  #endif
}
void loop() {
  #ifdef TINYUSB_NEED_POLLING_TASK
  // Manual call tud_task since it isn't called by Core's background
  TinyUSBDevice.task();
  #endif

  ArduinoOTA.handle();
  server.handleClient();

  // toggle LED every second
#ifdef LED_BUILTIN
  static unsigned long lastToggle = 0;
  static bool ledOn = false;

  if (millis() - lastToggle >= 1000) {
    lastToggle = millis();
    ledOn = !ledOn;
    digitalWrite(LED_BUILTIN, ledOn ? HIGH : LOW);
  }
#endif
}
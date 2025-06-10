#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <Arduino.h>
#include <LittleFS.h>
#include <map>

// Global configuration storage
extern std::map<String, String> deviceConfig;

// Function declarations
bool loadConfiguration();
String getConfigValue(const String& key);
void printConfiguration();

// Implementation
std::map<String, String> deviceConfig;

bool loadConfiguration() {
  deviceConfig.clear();

  if (!LittleFS.begin()) {
    Serial.println("LittleFS mount failed");
    return false;
  }

  File configFile = LittleFS.open("/config.txt", "r");
  if (!configFile) {
    Serial.println("Failed to open config.txt");
    return false;
  }

  while (configFile.available()) {
    String line = configFile.readStringUntil('\n');
    int separatorIndex = line.indexOf('=');
    if (separatorIndex > 0) {
      String key = line.substring(0, separatorIndex);
      String value = line.substring(separatorIndex + 1);
      value.trim();
      deviceConfig[key] = value;
    }
  }

  configFile.close();
  return true;
}

String getConfigValue(const String& key) {
  return deviceConfig.count(key) ? deviceConfig[key] : "";
}

void printConfiguration() {
    for (const auto& pair : deviceConfig) {
      Serial.print("Key: '");
      Serial.print(pair.first);
      Serial.print("' => Value: '");
      Serial.print(pair.second);
      Serial.println("'");
    }
}

#endif
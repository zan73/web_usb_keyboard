#ifndef SLACK_NOTIFIER_H
#define SLACK_NOTIFIER_H

#include <Arduino.h>
#include <WiFiClientSecure.h>

// Function declarations
void sendSlackNotification(const String& message, const char* webhookPath);

// Implementation
void sendSlackNotification(const String& message, const char* webhookPath) {
  const char* slackHost = "hooks.slack.com";
  const int slackPort = 443;

  WiFiClientSecure client;
  client.setInsecure(); // Skip SSL certificate verification

  // Connect to Slack
  if (!client.connect(slackHost, slackPort)) {
    #ifdef DEBUG
      Serial.println("Failed to connect to Slack");
    #endif
    return;
  }

  // Compose Slack message payload
  String payload = "{\"text\": \"" + message + "\"}";

  // Send POST request
  client.printf("POST %s HTTP/1.1\r\n", webhookPath);
  client.printf("Host: %s\r\n", slackHost);
  client.println("Content-Type: application/json");
  client.printf("Content-Length: %d\r\n", payload.length());
  client.println("Connection: close\r\n");
  client.println(payload);

  // Read response headers
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

  client.stop();
}

#endif
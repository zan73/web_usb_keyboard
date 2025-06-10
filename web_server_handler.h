#ifndef WEB_SERVER_HANDLER_H
#define WEB_SERVER_HANDLER_H

#include <Arduino.h>
#include <WebServer.h>
#include "config_manager.h"
#include "keyboard_handler.h"
#include "slack_notifier.h"
#include "security_manager.h"

// Global web server instance
extern WebServer webServer;

// Constants
extern const char* AUTH_REALM;
extern String AUTH_FAIL_RESPONSE;

// Function declarations
void initializeWebServer();
void handleWebServerClient();
void handleMainPage();
void handleKeystrokeSend();

// Implementation
WebServer webServer(80);
const char* AUTH_REALM = "Device Auth Realm";
String AUTH_FAIL_RESPONSE = "Authentication Failed";

void handleMainPage() {
  String clientIP = webServer.client().remoteIP().toString();
  String pageName = getConfigValue("pagename");
  String page = pageName.isEmpty() ? "/" : "/" + pageName;
  String slackWebhook = getConfigValue("slack_webhook");

  // Check if client is blocked
  if (isClientBlocked(clientIP)) {
    webServer.send(403, "text/plain", "Access blocked - too many failed authentication attempts.");
    return;
  }

  // Authenticate user
  if (!webServer.authenticate(getConfigValue("username").c_str(), getConfigValue("userpass").c_str())) {
    // Record failed attempt
    recordFailedAttempt(clientIP);
    
    if (!slackWebhook.isEmpty()) {
      // Send notification to Slack for monitoring
      sendSlackNotification("Authentication failed for IP: " + clientIP + " on page " + page, slackWebhook.c_str());
    }

    // Check if we should block this client
    if (getFailedAttemptCount(clientIP) >= MAX_FAILED_ATTEMPTS) {
      blockClient(clientIP);
      
      if (!slackWebhook.isEmpty()) {
        // Send notification to Slack for monitoring
        sendSlackNotification("Failed authentication attempt threshold reached for IP: " + clientIP + " on page " + page, slackWebhook.c_str());
      }
      
      webServer.send(403, "text/plain", "Too many failed authentication attempts - Access blocked");
      return;
    }
    
    webServer.requestAuthentication(DIGEST_AUTH, AUTH_REALM, AUTH_FAIL_RESPONSE);
    return;
  }

  // Successful authentication
  clearFailedAttempts(clientIP);
  
  if (!slackWebhook.isEmpty()) {
    // Send notification to Slack for monitoring
    sendSlackNotification("Successful authentication for IP: " + clientIP + " on page " + page, slackWebhook.c_str());
  }

  // Send the main HTML page
  webServer.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>USB Keyboard Controller</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; }
        .container { max-width: 600px; margin: 0 auto; }
        input[type="text"] { width: 70%; padding: 10px; margin: 5px; }
        input[type="submit"] { padding: 10px 20px; margin: 5px; }
        h2 { color: #333; }
    </style>
</head>
<body>
    <div class="container">
        <h2>USB Keyboard Controller</h2>
        <p>Send keystrokes via USB connection</p>
        <form action="/send" method="POST">
            <input type="text" name="keystroke" placeholder="Enter keystroke sequence" required>
            <input type="submit" value="Send Keystrokes">
        </form>
        <div style="margin-top: 20px;">
            <small>
                Examples: "Hello World", "CTRL+C", "CTRL+ALT+DEL", "F1", "ENTER"
            </small>
        </div>
    </div>
</body>
</html>
  )rawliteral");
}

void handleKeystrokeSend() {
  String clientIP = webServer.client().remoteIP().toString();
  String slackWebhook = getConfigValue("slack_webhook");

  // Authenticate user for POST request
  if (!webServer.authenticate(getConfigValue("username").c_str(), getConfigValue("userpass").c_str())) {
    webServer.send(403, "text/plain", "Authentication required");
    return;
  }

  // Get the keystroke data from POST request
  String keystrokeData = webServer.arg("keystroke");
  
  if (keystrokeData.length() == 0) {
    webServer.send(400, "text/plain", "No keystroke data provided");
    return;
  }

  // Send the keystrokes
  sendKeystrokeSequence(keystrokeData);
  
  // Send response
  webServer.send(200, "text/plain", "Keystrokes sent: " + keystrokeData);
  
  // Log to Slack if configured
  if (!slackWebhook.isEmpty()) {
    // Send notification to Slack for monitoring
    sendSlackNotification("Keystrokes sent by IP: " + clientIP + " - Data: '" + keystrokeData + "'", slackWebhook.c_str());
  }

  #ifdef DEBUG
    Serial.println("Sent keystrokes: " + keystrokeData + " from IP: " + clientIP);
  #endif
}

void initializeWebServer() {
  String pageName = getConfigValue("pagename");
  String page = pageName.isEmpty() ? "/" : "/" + pageName;
  
  // Set up main page handler
  webServer.on(page, handleMainPage);
  
  // Set up keystroke send handler
  webServer.on("/send", HTTP_POST, handleKeystrokeSend);
  
  // Start the web server
  webServer.begin();
  
  #ifdef DEBUG
    Serial.println("Web server initialized");
    Serial.println("Main page: " + page);
    Serial.println("Send endpoint: /send");
  #endif
}

void handleWebServerClient() {
  webServer.handleClient();
  
  // Periodically clean up expired blocked IPs
  static unsigned long lastCleanup = 0;
  if (millis() - lastCleanup > 300000) { // Clean up every 5 minutes
    unblockExpiredIPs();
    lastCleanup = millis();
  }
}

#endif
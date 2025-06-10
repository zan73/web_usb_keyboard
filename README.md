# Arduino Web-based USB Keystroke Sender

A secure web-based interface for remotely sending keystrokes to USB-connected devices. This project enables remote keyboard input through a simple web interface, tested and optimized for the Raspberry Pi Pico 2W.

## Key Features

- **Remote Keystroke Control**: Send keyboard inputs to USB-connected devices via web interface
- **Security**: Implements Digest Authentication over HTTP (optimized for microcontroller constraints)
- **Visual Feedback**: LED status indicator toggles to provide visual confirmation that the device is active and processing commands
- **Monitoring**: Slack webhook integration for login attempt and keystroke activity notifications
- **Easy Configuration**: All settings (WiFi credentials, authentication, Slack webhook) managed through a single config file
- **Raspberry Pi Pico 2W Optimized**: Designed to work within the processing and memory limitations of the Pico 2W

## Setup

Configuration is handled via a `config.txt` file that contains:
- WiFi network credentials
- Web authentication settings
- Slack webhook details

Upload the configuration file to your device using the [Arduino LittleFS Upload Plugin](https://github.com/earlephilhower/arduino-littlefs-upload).

## Architecture Notes

The implementation uses HTTP with Digest Authentication rather than HTTPS due to the processing constraints of the Pico 2W, while still providing reasonable security for the authentication process.

## File Structure

### Core Files

1. **main.ino** - Main Arduino sketch file
   - Contains setup() and loop() functions
   - Initializes all components
   - Handles WiFi connection and OTA updates

2. **config_manager.h** - Configuration Management
   - Loads configuration from LittleFS (/config.txt)
   - Provides access to configuration values
   - Debug configuration printing

3. **keyboard_handler.h** - Keyboard Input Processing
   - HID keyboard initialization and management
   - ASCII to HID keycode conversion
   - Keystroke sequence processing (including key combinations)
   - Support for special keys and modifiers

4. **slack_notifier.h** - Slack Notification Handler
   - Sends notifications to Slack webhook
   - Handles HTTPS connections to Slack API
   - Configurable via webhook URL in config.txt

5. **security_manager.h** - Security and Authentication
   - Failed login attempt tracking
   - IP blocking functionality
   - Automatic unblocking after timeout period
   - Configurable attempt limits and block duration

6. **web_server_handler.h** - Web Server Management
   - HTTP server setup and request handling
   - Authentication integration
   - HTML interface for keystroke input
   - POST endpoint for keystroke processing

## Configuration File (config.txt)

Create a file named `config.txt` in the LittleFS filesystem with the following format:

```
ssid=YourWiFiSSID
password=YourWiFiPassword
username=admin
userpass=yourpassword
pagename=keyboard
slack_webhook=/services/YOUR/SLACK/WEBHOOK/PATH
```

### Configuration Parameters

- `ssid` - WiFi network name
- `password` - WiFi network password  
- `username` - Web interface username
- `userpass` - Web interface password
- `pagename` - Custom page name (optional, defaults to "/")
- `slack_webhook` - Slack webhook path (optional)

## Key Features

### Security Features
- HTTP Digest Authentication
- Failed attempt tracking (max 3 attempts)
- Automatic IP blocking (60 minutes)
- Slack notifications for security events

### Keyboard Features
- Support for all standard keys and modifiers
- Key combinations (e.g., CTRL+ALT+DEL)
- Special keys (Function keys, arrows, etc.)
- ASCII character input with automatic shift handling

### Supported Key Formats
- **Regular text**: `Hello World`
- **Special keys**: `ENTER`, `ESC`, `TAB`, `BACKSPACE`
- **Function keys**: `F1`, `F2`, ..., `F24`
- **Modifiers**: `CTRL`, `ALT`, `SHIFT`, `WIN`/`GUI`
- **Key combinations**: `CTRL+C`, `CTRL+ALT+DEL`
- **Arrow keys**: `UP`, `DOWN`, `LEFT`, `RIGHT`
- **Keypad**: `KP1`, `KP2`, `KPADD`, `KPENTER`

## Usage

1. Upload the code to your Arduino-compatible device
2. Copy `config.txt.template` to `config.txt` and update with your actual credentials
3. Upload to your device using the LittleFS upload plugin

2. Create and upload the config.txt file to LittleFS
3. Connect the device via USB to the target computer
4. Access the web interface at `http://[device-ip]/[pagename]`
5. Enter keystrokes and click "Send Keystrokes"

## Hardware Requirements

- Arduino-compatible board with USB HID capability
- WiFi connectivity
- USB connection to target computer

## Dependencies

- Adafruit TinyUSB Library
- ESP32/Arduino WiFi libraries
- LittleFS filesystem support
- ArduinoOTA library

## Debug Mode

Uncomment `#define DEBUG` in main.ino to enable debug output via Serial Monitor.
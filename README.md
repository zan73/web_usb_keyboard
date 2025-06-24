# Arduino Web-based USB Keystroke Sender

A secure web-based interface for remotely sending keystrokes to USB-connected devices.
Works with MacOS FileVault (Cold boot/FileVault) login screen as a "Boot-mode" keyboard.
This project enables remote keyboard input through a simple web interface, tested and optimized for the Raspberry Pi Pico 2W.

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

1. Install Arduino IDE 2.x and then add Arduino-Pico via [these instructions](https://arduino-pico.readthedocs.io/en/latest/install.html)
2. From the Tools menu, select USB Stack: Adafruit TinyUSB and Flash Size: that allows for some Filesystem storage (minimum is FS: 64KB). Leave Wifi Region set to Worldwide.
3. Click the Upload button to upload the code to your Arduino-compatible device
4. Copy `config.txt.template` to `config.txt` and update with your actual credentials
5. Upload to your device using the Arduino LittleFS Upload Plugin. Installation and upload steps are on [this page](https://github.com/earlephilhower/arduino-littlefs-upload).
6. Connect the device via USB to the target computer
7. Access the web interface at `http://[device-ip]/[pagename]`
8. Enter keystrokes and click "Send Keystrokes"

## Hardware Requirements

- Arduino-compatible board with USB HID capability (Tested on Raspberry Pi Pico 2W)
- WiFi connectivity
- USB connection to target computer

## Dependencies

- Adafruit TinyUSB Library
- ESP32/Arduino WiFi libraries
- LittleFS filesystem support
- ArduinoOTA library

## Debug Mode

Uncomment `#define DEBUG` in web_usb_keyboard.ino to enable debug output via Serial Monitor.
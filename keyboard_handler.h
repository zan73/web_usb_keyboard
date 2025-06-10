#ifndef KEYBOARD_HANDLER_H
#define KEYBOARD_HANDLER_H

#include <Arduino.h>
#include "Adafruit_TinyUSB.h"
#include <map>

// HID key structure
typedef struct {
  uint8_t modifier;
  uint8_t keycode;
} HidKey;

// Global USB HID object
extern Adafruit_USBD_HID usbHid;

// Function declarations
void initializeKeyboard();
void sendKeystrokeSequence(const String& input);
HidKey convertAsciiToHid(char character);

// Implementation
Adafruit_USBD_HID usbHid;

// HID report descriptor using TinyUSB's template
uint8_t const hidReportDescriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD()
};

// Modifier key mappings
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

// Special key mappings
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
  {"PLUS",       {KEYBOARD_MODIFIER_LEFTSHIFT, HID_KEY_EQUAL}},

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

void initializeKeyboard() {
  usbHid.setBootProtocol(HID_ITF_PROTOCOL_KEYBOARD);
  usbHid.setPollInterval(2);
  usbHid.setReportDescriptor(hidReportDescriptor, sizeof(hidReportDescriptor));
  usbHid.setStringDescriptor("TinyUSB Keyboard");
  usbHid.begin();
}

HidKey convertAsciiToHid(char character) {
  HidKey result = {0, 0};

  if (character >= 'a' && character <= 'z') {
    result.keycode = HID_KEY_A + (character - 'a');
    result.modifier = 0;
  } else if (character >= 'A' && character <= 'Z') {
    result.keycode = HID_KEY_A + (character - 'A');
    result.modifier = KEYBOARD_MODIFIER_LEFTSHIFT;
  } else if (character >= '1' && character <= '9') {
    result.keycode = HID_KEY_1 + (character - '1');
    result.modifier = 0;
  } else if (character == '0') {
    result.keycode = HID_KEY_0;
    result.modifier = 0;
  } else {
    // Handle symbols and punctuation
    switch (character) {
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

void sendKeystrokeSequence(const String& input) {
  uint8_t const reportId = 0;

  // Split input by spaces
  int startPos = 0;
  while (startPos < input.length()) {
    int endPos = input.indexOf(' ', startPos);
    if (endPos == -1) endPos = input.length();
    String segment = input.substring(startPos, endPos);
    segment.trim();

    if (segment.length() == 0) {
      startPos = endPos + 1;
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

        // Check if it's a modifier key
        if (MODIFIER_KEYS.count(key)) {
          modifier |= MODIFIER_KEYS[key];
        } else {
          HidKey hk = {0, 0};
          if (SPECIAL_KEYS.count(key)) {
            hk = SPECIAL_KEYS[key];
          } else if (key.length() == 1) {
            hk = convertAsciiToHid(key.charAt(0));
          }

          if (hk.keycode != 0 && keyIndex < 6) {
            keycode[keyIndex++] = hk.keycode;
            modifier |= hk.modifier;
          }
        }
        chordStart = chordEnd + 1;
      }
      
      while (!usbHid.ready()) delay(1);
      usbHid.keyboardReport(reportId, modifier, keycode);
      
      #ifdef DEBUG
        Serial.printf("Modifier: %d, Keycodes: ", modifier);
        for (int i = 0; i < 6; ++i) {
          Serial.printf("%d ", keycode[i]);
        }
        Serial.println();
      #endif
      
      delay(5);
      usbHid.keyboardRelease(reportId);
      delay(5);

    } else {
      // Handle special key or character sequence
      String key = segment;
      key.trim();

      if (SPECIAL_KEYS.count(key)) {
        // Special key (e.g. ENTER, CTRL, etc.)
        HidKey hk = SPECIAL_KEYS[key];
        uint8_t keycode[6] = {hk.keycode};
        
        while (!usbHid.ready()) delay(1);
        usbHid.keyboardReport(reportId, hk.modifier, keycode);
        
        #ifdef DEBUG
          Serial.printf("Special key - Modifier: %d, Keycode: %d\n", hk.modifier, hk.keycode);
        #endif
        
        delay(5);
        usbHid.keyboardRelease(reportId);
        delay(5);
      } else {
        // Treat as a sequence of characters
        for (size_t i = 0; i < key.length(); ++i) {
          HidKey hk = convertAsciiToHid(key.charAt(i));
          if (hk.keycode == 0) continue; // skip unsupported chars

          uint8_t keycode[6] = {hk.keycode};
          while (!usbHid.ready()) delay(1);
          usbHid.keyboardReport(reportId, hk.modifier, keycode);
          
          #ifdef DEBUG
            Serial.printf("Character '%c' -> Modifier: %d, Keycode: %d\n", key.charAt(i), hk.modifier, hk.keycode);
          #endif
          
          delay(5);
          usbHid.keyboardRelease(reportId);
          delay(5);
        }
      }
    }
    startPos = endPos + 1;
  }
}

#endif
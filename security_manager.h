#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <Arduino.h>
#include <map>

// Security configuration constants
#define MAX_FAILED_ATTEMPTS 3
#define BLOCK_DURATION_MS (60 * 60 * 1000) // 60 minutes in milliseconds

// Global security state
extern std::map<String, int> clientFailedAttempts;
extern std::map<String, unsigned long> blockedClientIPs;

// Function declarations
bool isClientBlocked(const String& clientIP);
void recordFailedAttempt(const String& clientIP);
void blockClient(const String& clientIP);
void clearFailedAttempts(const String& clientIP);
void unblockExpiredIPs();

// Implementation
std::map<String, int> clientFailedAttempts;
std::map<String, unsigned long> blockedClientIPs;

bool isClientBlocked(const String& clientIP) {
  if (!blockedClientIPs.count(clientIP)) {
    return false;
  }
  
  unsigned long currentTime = millis();
  unsigned long blockTime = blockedClientIPs[clientIP];
  
  // Check if block has expired
  if (currentTime - blockTime >= BLOCK_DURATION_MS) {
    blockedClientIPs.erase(clientIP);
    return false;
  }
  
  return true;
}

void recordFailedAttempt(const String& clientIP) {
  if (clientFailedAttempts.count(clientIP)) {
    clientFailedAttempts[clientIP]++;
  } else {
    clientFailedAttempts[clientIP] = 1;
  }
}

void blockClient(const String& clientIP) {
  blockedClientIPs[clientIP] = millis();
  clientFailedAttempts.erase(clientIP); // Reset count after blocking
}

void clearFailedAttempts(const String& clientIP) {
  clientFailedAttempts.erase(clientIP);
}

int getFailedAttemptCount(const String& clientIP) {
  return clientFailedAttempts.count(clientIP) ? clientFailedAttempts[clientIP] : 0;
}

void unblockExpiredIPs() {
  unsigned long currentTime = millis();
  
  auto it = blockedClientIPs.begin();
  while (it != blockedClientIPs.end()) {
    if (currentTime - it->second >= BLOCK_DURATION_MS) {
      it = blockedClientIPs.erase(it);
    } else {
      ++it;
    }
  }
}

#endif
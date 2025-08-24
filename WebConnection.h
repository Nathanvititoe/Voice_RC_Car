#pragma once
#include <Arduino.h>
#include <WiFiS3.h>                   // UNO R4 WiFi networking
#include "secrets/arduino_secrets.h"  // config env vars

#ifndef SECRET_SSID
#error "SSID not defined in configuration"
#endif

#ifndef SECRET_PASS
#error "Network password not defined in configuration"
#endif

#ifndef RETRY_MS
#error "retry ms not defined in configuration"
#endif

#ifndef CONNECTION_PORT
#define CONNECTION_PORT 9000
#endif

#ifndef READ_TIMEOUT_MS
#define READ_TIMEOUT_MS 3000
#endif

class WebConnection {
public:
  WebConnection();

  // station mode, join local network
  bool begin();

  // handles reconnects and client acception (called in loop)
  bool poll(String& incomingCmd, uint32_t readTimeoutMs = READ_TIMEOUT_MS);

  IPAddress localIP() const {
    return WiFi.localIP();
  }

private:
  bool _attemptConnection(uint32_t connectWindowMs);

  uint16_t _port;
  uint32_t _retryMs;
  unsigned long _nextRetryAt = 0;
  bool _serverStarted = false;                // track server state
  IPAddress _lastIP = IPAddress(0, 0, 0, 0);  // last IP recorded
  WiFiServer _server;
};

#include "WebConnection.h"

/*
* Web Connection Constructor
*/
WebConnection::WebConnection()
  : _port(static_cast<uint16_t>(CONNECTION_PORT)),
    _retryMs(static_cast<uint32_t>(RETRY_MS)),
    _server(_port) {}

/*
* Attempts to connect to Wifi for 'connectWindowMs' ms
* @param connectWindowMs - window to attempt connection
*/
bool WebConnection::_attemptConnection(uint32_t connectWindowMs) {
  // check for wifi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found.");
    while (true) { delay(1000); }
  }

  unsigned long startTime = millis();                    // start timer
  if (WiFi.status() != WL_CONNECTED) WiFi.disconnect();  // start fresh
  WiFi.begin(SECRET_SSID, SECRET_PASS);

  // while we are below connectWindowMs time
  while ((millis() - startTime) < connectWindowMs && WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print('.');
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println();
    return false;
  }

  // We have link; wait briefly for a DHCP lease
  IPAddress ip;
  unsigned long t1 = millis();
  // keep retrying until we get an IP
  do {
    ip = WiFi.localIP();
    if (ip != IPAddress(0, 0, 0, 0)) break;
    delay(200);
  } while ((millis() - t1) < 4000);

  if (ip == IPAddress(0, 0, 0, 0)) {
    Serial.println();
    Serial.println("Connected to AP but no IP yet.");
    return false;
  }

  Serial.println();
  _lastIP = ip;
  Serial.print("WiFi Connected. IP Address : ");
  Serial.println(ip);
  return true;
}

/*
* Init Connection, Start Web Server
*/
bool WebConnection::begin() {
  Serial.print("Connecting to ");
  Serial.println(SECRET_SSID);

  // block anything until connection
  while (!_attemptConnection(15000)) {  // Try 15s for connection initially
    Serial.println("Retrying Connection...");
    delay(_retryMs ? _retryMs : 2000);
  }

  // start web server if its not started already
  if (!_serverStarted) {
    _server.begin();
    Serial.println("Server Started!");
    _serverStarted = true;
  }
  Serial.print("Listening on port ");
  Serial.println(_port);
  return true;
}

/*
* Poll Connection for commands
* @param incomingCmd - incoming message from PC
* @param readTimeoutMs - time to wait for msg before failing
*/
bool WebConnection::poll(String& incomingCmd, uint32_t readTimeoutMs) {
  // if not connected, retry connection
  if (WiFi.status() != WL_CONNECTED) {
    if (millis() >= _nextRetryAt) {
      Serial.println("WiFi down. Retrying…");
      (void)_attemptConnection(5000);
      _nextRetryAt = millis() + _retryMs;
    }
  }

  // check client is available
  WiFiClient client = _server.available();
  if (!client) return false;

  client.setTimeout(readTimeoutMs);

  // read incoming message until new line
  String msg = client.readStringUntil('\n');
  msg.trim();  // cleanup msg

  // read msg, return "Received: 'msg" to client
  if (msg.length() > 0) {
    incomingCmd = msg;
    client.print("Received: ");
    client.print(msg);
  } else {
    Serial.println("Msg Empty or timed out.");
    client.println("EMPTY");
  }

  client.stop();
  return (msg.length() > 0);
}

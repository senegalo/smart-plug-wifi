#include <Arduino.h>
#include "ESP8266WebServer.h"
#include "ESP8266WiFi.h"
#include "WiFiClient.h"

const char* ssid = "";
const char* password = "";

ESP8266WebServer server(80);

void relay(int pin, int level) {
  Serial.printf("Setting pin %d to %d\n", pin, level);
  if (pin == 0) {
    pin = 0;
  } else if (pin == 1) {
    pin = 2;
  }
  if (level == 1) {
    digitalWrite(pin, LOW);
  } else if (level == 0) {
    digitalWrite(pin, HIGH);
  }
}

void handleError() {
  server.send(500, "text/plain",
              "Requires pin and level params and delay param in case of "
              "delayed calls.");
}

void handleRelay() {
  if (server.hasArg("pin") && server.hasArg("level")) {
    int pin = server.arg("pin").toInt();
    int level = server.arg("level").toInt();
    relay(pin, level);
    server.send(200, "text/plain", "OK");
  } else {
    handleError();
  }
  delay(100);
}

void delayHandleRelay() {
  if (server.hasArg("pin") && server.hasArg("level") &&
      server.hasArg("delay")) {
    int delayTime = server.arg("delay").toInt();
    int pin = server.arg("pin").toInt();
    int level = server.arg("level").toInt();

    server.send(200, "text/plain", "OK");

    Serial.println("Delaying to " + server.arg("delay"));
    delay(delayTime);
    relay(pin, level);
  } else {
    handleError();
  }
  delay(100);
}

void info() {
  String out =
      "<html><head></head><body>"
      "<a href='http://printer-relays/relay?pin=0&level=1'>Turn On Relay 0 "
      "</a><br/><a href='http://printer-relays/relay?pin=1&level=1'>Turn On "
      "Relay 1</a><br/><a "
      "href='http://printer-relays/relay?pin=0&level=0'>Turn Off "
      "Relay 0</a><br/><a "
      "href='http://printer-relays/relay?pin=1&level=0'>Turn Off "
      "Relay 1</a></body></html>";

  server.send(200, "text/html", out);
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.hostname("printer-relays");
  WiFi.begin(ssid, password);
  Serial.print("\n\r \n\rWorking to connect");

  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);

  digitalWrite(0, HIGH);
  digitalWrite(2, HIGH);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("DHT Weather Reading Server");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", []() { info(); });
  server.on("/relay", []() { handleRelay(); });
  server.on("/delayed/relay", []() { delayHandleRelay(); });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() { server.handleClient(); }

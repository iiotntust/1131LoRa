#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

const char* ssid = "Factory2_2.4G";
const char* password = "118factory2";

const char* mqttServer = "broker.emqx.io";
const char* clientId   = "d1miniClient";
const char* topicID    = "LoRaID";
const char* topicTemp  = "LoRaTemp";
const char* topicHumi  = "LoRaHumi";

WiFiClient espClient;
PubSubClient client(espClient);

#define LORA_CS   D8
#define LORA_RST  D0
#define LORA_IRQ  D2

void reconnect() {
  while (!client.connected()) {
    Serial.print("MQTT Connecting...");
    if (client.connect(clientId)) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  WiFi.begin(ssid, password);
  Serial.print("Connecting WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nWiFi Connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, 1883);

  Serial.println("Initializing LoRa...");
  SPI.begin();
  LoRa.setPins(LORA_CS, LORA_RST, LORA_IRQ);

  if (!LoRa.begin(915E6)) 
  {      
    Serial.println("LoRa init failed!");
    while (1);
  }
  LoRa.setSignalBandwidth(250E3);
  Serial.println("LoRa Ready!");
}

void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();

  int packetSize = LoRa.parsePacket();
  if (packetSize) 
  {
    String raw = "";
    while (LoRa.available()) 
    {
      raw += (char)LoRa.read();
    }

    Serial.print("LoRa Received Raw: ");
    Serial.println(raw);

    StaticJsonDocument<256> doc;
    DeserializationError err = deserializeJson(doc, raw);
    if (err) 
    {
      Serial.print("JSON Error: ");
      Serial.println(err.c_str());
      return;
    }

    int id       = doc["ID"];
    float temp   = doc["Temp"];
    float humi   = doc["Humi"];
    char buf[20];
    snprintf(buf, sizeof(buf), "%d", id);
    client.publish(topicID, buf);
    dtostrf(temp, 4, 2, buf);
    client.publish(topicTemp, buf);
    dtostrf(humi, 4, 2, buf);
    client.publish(topicHumi, buf);
    Serial.print("Published ID: ");
    Serial.print(id);
    Serial.print("  Temp: ");
    Serial.print(temp);
    Serial.print("  Humi: ");
    Serial.println(humi);
  }
}


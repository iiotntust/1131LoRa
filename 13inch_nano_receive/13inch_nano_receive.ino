#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
//#include <string.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
//#include <DHT.h>
// 設定OLED
#define OLED_RESET -1
Adafruit_SH1106 display(OLED_RESET);// For 13inch screen
/*
OLED    | Nano
------------
GND     | GND
5V      | 5V
SCK/SCL | A5
SDA     | A4
*/

#if (SH1106_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SH1106.h!");
#endif

float t;
float h;

String istring;
void setup() {
  display.begin(SH1106_SWITCHCAPVCC, 0x3c); 
  Serial.begin(9600);
  display.display();
 // while (!Serial);
  Serial.println(F("LoRa Receiver"));
 // LoRa.setTxPower(17);

  if (!LoRa.begin(924E6)) {
    Serial.println(F("Starting LoRa failed!"));
    while (1);
  }
  // Bandwidth
  LoRa.setSignalBandwidth(250E3); 
  delay(100);  
  Serial.println(F("MiniLoRa")); 
  display.clearDisplay(); 
  display.display();
  display.setTextSize(1);                   //設定文字大小
  display.setTextColor(WHITE);              //預設顏色
  display.println(F("MiniLoRa"));           //同樣是顯示字串
  display.println(F("Staring."));
  display.display();                        //要有這列才會把文字顯示出來
  delay(5000);
}

void loop() {
  display.clearDisplay(); 
  String istring="";
  String temp = "";
  String humi = "";
  String c = "";
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
  Serial.print(F("Received packet, "));
  long rssi = LoRa.packetRssi();
   Serial.print(F("RSSI : "));
   Serial.println(rssi);
    display.setTextSize(2);
    display.setCursor(0,0);
    display.print(F("RSSI:"));
    display.println(rssi);
    // read packet
    while (LoRa.available()) {
       c = LoRa.readString();
       Serial.print(c);
      //istring = istring + ((char)LoRa.read());
    }
    temp = c.substring(17,22);
    humi = c.substring(30,35);
    //Serial.println(temp);
    //Serial.println(humi);
    //display.print(F("istring"));
    display.setTextSize(1);
    //display.println(c);
    display.print(F("temp:"));
    display.println(temp);
    display.print(F("humi:"));
    display.println(humi);
    display.display();   
    delay(1);
  }
}

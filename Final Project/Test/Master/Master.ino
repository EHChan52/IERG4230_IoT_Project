#include <SPI.h>
#include <Wire.h>
#include "AM2320.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

AM2320 th;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int BH1750address = 0x23; // BH1750 I2C address
byte buff[2];
int sensorlightintensityue;

const char* ssid = "Edward"; // replace with your WiFi SSID
const char* password = "12345678"; // replace with your WiFi password
unsigned long masterChannelNumber = 2787975; // replace with your ThingSpeak channel number
unsigned long slaveChannelNumber = 2787819; // replace with your ThingSpeak channel number
const char* masterWriteAPIKey = "WWZW447IN3G56NJV"; // replace with your ThingSpeak Write API Key
const char* slaveReadAPIKey = "CKKSKLF3FM82JKMY"; // replace with your ThingSpeak Read API Key
int statusCode = 0;
int field[5] = {1,2,3,4,5};
float TempGet = 0;
float HumiGet = 0;
float LumenGet = 0;
float GasInteGet = 0;

WiFiClient client;

void setup() {
  Wire.begin(2, 0);
  Serial.begin(115200);

  Serial.println("ESP8266-12E/F BH1750 test program\n");
  Serial.println("Build-in LED at GPIO-16(D0)");
  Serial.println("I2C SDA Pin at GPIO-2(D4)");
  Serial.println("I2C SCL Pin at GPIO-0(D3)");

  // OLED Setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Connecting");
  display.println("to");
  display.println("Internet..");
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  display.clearDisplay();
  display.println("Device");
  display.println("Connected");

  ThingSpeak.begin(client);

  delay(300);
}

void loop() {
  // Read and display AM2320 data
  float localhumidity = th.getHumidity();
  float localtemperature = th.getTemperature();
  Serial.print("humidity: ");
  Serial.print(localhumidity);
  Serial.print("%");
  Serial.print(" || temperature: ");
  Serial.print(localtemperature);
  Serial.println("*C");

  // Read and display BH1750 data
  uint16_t locallightintensity = 0;
  BH1750_Init(BH1750address);
  delay(200);
  if (2 == BH1750_Read(BH1750address)) {
    locallightintensity = ((buff[0] << 8) | buff[1]) / 1.2;
    Serial.print(locallightintensity, DEC);
    Serial.println("[lx]");
  }

  // Read and display analog sensor lightintensityue
  uint16_t localsensorgasintensity = analogRead(A0);
  Serial.print("PPM =");
  Serial.println(localsensorgasintensity);
  
  // Upload data to ThingSpeak Master Channel
  ThingSpeak.setField(1, localtemperature);
  ThingSpeak.setField(2, localhumidity);
  ThingSpeak.setField(3, locallightintensity);
  ThingSpeak.setField(4, localsensorgasintensity);
  ThingSpeak.writeFields(masterChannelNumber, masterWriteAPIKey);

  //get data from thingspeak channel
  statusCode = ThingSpeak.readMultipleFields(slaveChannelNumber,slaveReadAPIKey);
  if (statusCode == 200){
    //success
    Serial.println("Successful Read Fields");
    TempGet = ThingSpeak.getFieldAsFloat(field[0]);
    HumiGet = ThingSpeak.getFieldAsFloat(field[1]);
    LumenGet = ThingSpeak.getFieldAsFloat(field[2]);
    GasInteGet = ThingSpeak.getFieldAsFloat(field[3]);
    Serial.print("Temperature: ");
    Serial.println(TempGet);
    Serial.print("Humidity: ");
    Serial.println(HumiGet);
    Serial.print("Light Intensity: ");
    Serial.println(LumenGet);
    Serial.print("Gas Intensity: ");
    Serial.println(GasInteGet);
  }
  else{
     Serial.println("Problem reading channel. HTTP error code " + String(statusCode)); 
  }

  // OLED display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.println("Module 1:");
  display.setCursor(0, 30);
  if(localtemperature < 15 || localtemperature > 30 || localhumidity > 75 || localsensorgasintensity > 600 || locallightintensity < 30 || locallightintensity > 300){
    if(localtemperature < 10){
      display.println("too cold!");
      display.display();
    }
    if(localtemperature > 30){
      display.println("too hot!");
      display.display();
    }
    if(localhumidity > 75){
      display.println("too humid!");
      display.display();
    }
    if(localsensorgasintensity > 600){
      display.println("food rotted!");
      display.display();
    }
    if(locallightintensity < 30){
      display.println("too dark!");
      display.display();
    }
    if(locallightintensity > 300){
      display.println("too bright!");
      display.display();
    }
  }
  else{
    display.println("Normal");
    display.display();
  }
  delay(3000);

  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Module 2:");
  display.setCursor(0, 30);
  if(localtemperature < 15 || localtemperature > 30 || localhumidity > 75 || localsensorgasintensity > 600 || locallightintensity < 30 || locallightintensity > 300){
    if(TempGet < 15){
      display.println("too cold!");
      display.display();
    }
    if(TempGet > 30){
      display.println("too hot!");
      display.display();
    }
    if(HumiGet > 75){
      display.println("too humid!");
      display.display();
    }
    if(GasInteGet > 600){
      display.println("food rotted!");
      display.display();
    }
    if(LumenGet < 30){
      display.println("too dark!");
      display.display();
    }
    if(LumenGet > 300){
      display.println("too bright!");
      display.display();
    }
  }
  else{
    display.println("Normal");
    display.display();
  }
  delay(3000);
}

int BH1750_Read(int address) {
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available()) {
    buff[i] = Wire.read();
    i++;
  }
  Wire.endTransmission();
  return i;
}

void BH1750_Init(int address) {
  Wire.beginTransmission(address);
  Wire.write(0x10); // 1lx resolution 120ms
  Wire.endTransmission();
}


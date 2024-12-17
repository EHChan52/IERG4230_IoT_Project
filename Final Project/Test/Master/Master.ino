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
unsigned long myChannelNumber = 2787819; // replace with your ThingSpeak channel number
const char* myWriteAPIKey = "VYHVNUW4JGR8SJJQ"; // replace with your ThingSpeak Write API Key

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
  float humidity = th.getHumidity();
  float temperature = th.getTemperature();
  Serial.print("humidity: ");
  Serial.print(humidity);
  Serial.print("%");
  Serial.print(" || temperature: ");
  Serial.print(temperature);
  Serial.println("*C");

  // Read and display BH1750 data
  uint16_t lightintensity = 0;
  BH1750_Init(BH1750address);
  delay(200);
  if (2 == BH1750_Read(BH1750address)) {
    lightintensity = ((buff[0] << 8) | buff[1]) / 1.2;
    Serial.print(lightintensity, DEC);
    Serial.println("[lx]");
  }

  // OLED display
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 10);
  display.println("Module 1:");
  display.setCursor(0, 30);
  display.println("Normal");
  display.display();
  delay(3000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("Module 2:");
  display.setCursor(0, 30);
  display.println("Need Check");
  display.display();
  delay(3000);

  // Read and display analog sensor lightintensityue
  Serial.println("--------------");
  sensorlightintensityue = analogRead(A0);
  Serial.print("PPM =");
  Serial.println(sensorlightintensityue);

  // Upload data to ThingSpeak
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.setField(3, lightintensity);
  ThingSpeak.setField(4, sensorlightintensityue);
  ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  delay(1000);
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

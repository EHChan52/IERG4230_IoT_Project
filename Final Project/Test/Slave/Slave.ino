#include <Wire.h>
#include "AM2320.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

AM2320 th;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int BH1750address = 0x23; // BH1750 I2C address
byte buff[2];
int sensorValue;

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
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("IERG4230 BH1750 Module");
  display.display();

  delay(300);
}

void loop() {
  // Read and display AM2320 data
  Serial.print("humidity: ");
  Serial.print(th.getHumidity());
  Serial.print("%");
  Serial.print(" || temperature: ");
  Serial.print(th.getTemperature());
  Serial.println("*C");

  // Read and display BH1750 data
  uint16_t val = 0;
  BH1750_Init(BH1750address);
  delay(200);
  if (2 == BH1750_Read(BH1750address)) {
    val = ((buff[0] << 8) | buff[1]) / 1.2;
    Serial.print(val, DEC);
    Serial.println("[lx]");
  }

  // OLED display
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println("IERG4230 BH1750 Module");
  display.setCursor(0, 30);
  display.println("Digital Ambient Light Sensor");
  display.setCursor(0, 50);
  display.print("lx = ");
  display.setCursor(30, 50);
  display.print(val);
  display.display();

  // Read and display analog sensor value
  Serial.println("--------------");
  sensorValue = analogRead(A0);
  Serial.print("PPM =");
  Serial.println(sensorValue);

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

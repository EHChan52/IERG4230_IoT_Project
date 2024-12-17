/*
   IERG4230 IoT Testing Program
   BH1750 Ambeient Light Sensor
   Matter Needing attention:
   On Boot/Reset/wakeup,
   GPIO15(D8) keep LOW and GPIO2(D4)keep HIGH
   GPIO0(D3) HIGH = Run mode / LOW = Flash mode
   On Board LEDs (active LOW):
    GPIO16(D0)
    GPIO2(D4)
  
  Connections:
  RCWL-0516    ESP8266-12E
  ---------    ---------
  VCC          Not connected
  GND          GND
  SCL          GPIO 0(D3)
  SDA          GPIO 2(D4)
  ADDR         Not connected
*/
#include <Wire.h> //I2C
#include <math.h>

#include <Adafruit_GFX.h>     //OLED
#include <Adafruit_SSD1306.h> //OLED

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// OLED
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, i2c Addr, Reset share with 8266 reset);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

int BH1750address = 0x23; // BH1750 I2C address

byte buff[2];

void setup()
{
  Wire.begin(2,0);
  Serial.begin(115200);

  Serial.println("ESP8266-12E/F BH1750 test program\n");
  Serial.println("Build-in LED at GPIO-16(D0)");
  Serial.println("I2C SDA Pin at GPIO-2(D4)");
  Serial.println("I2C SCL Pin at GPIO-0(D3)");
  
  //OLED Setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  //OLED diplay 1st line
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.println("IERG4230 BH1750 Module"); // Display static text
  display.display();

  delay(300);
}

void loop()
{
  int i;
  uint16_t val = 0;
  BH1750_Init(BH1750address);
  delay(200);
  if (2 == BH1750_Read(BH1750address))
  {
    val = ((buff[0] << 8) | buff[1]) / 1.2;
    Serial.print(val, DEC);
    Serial.println("[lx]");
  }
  display.clearDisplay();
  //OLED diplay 1st line
  display.setCursor(0, 10);
  display.println("IERG4230 BH1750 Module");  // Display static text
  
  //OLED diplay 3rd line
  display.setCursor(0, 30);
  display.println("Digital Ambient Light Sensor");   // Display static text

  //OLED diplay 5rd line
  display.setCursor(0, 50);
  display.print("lx = ");    // Display static text
  display.setCursor(30, 50);
  display.print(val);
  display.display();
  delay(150);
}

int BH1750_Read(int address) //
{
  int i = 0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while (Wire.available())
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  return i;
}

void BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}

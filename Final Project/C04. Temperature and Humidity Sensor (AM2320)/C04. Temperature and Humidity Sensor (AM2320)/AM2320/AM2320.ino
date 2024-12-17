#include <Wire.h>
#include "AM2320.h"
AM2320 th;

void setup() {
  Serial.begin(115200);
}

void loop() {
  //Serial.println("---------------");
  Serial.print("humidity: ");
  Serial.print(th.getHumidity());
  Serial.print("%");
  Serial.print("|| temperature: ");
  Serial.print(th.getTemperature());
  Serial.println("*C");
  Serial.println("---------------");
  delay(1000);
}

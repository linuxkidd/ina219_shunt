
#include "i2cCurrent.h"


i2cCurrent shunt;

const int intervalDisplay = 250;
unsigned long previousMillisDisplay = 0;
unsigned long currentMillis = 0;

char buffer[64];

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while(!Serial){}
  Serial.println("Hello - Arduino_INA219 !");
  delay(30);
  shunt.begin();
  delay(10);
}

void loop() {
  shunt.loop();
  currentMillis = millis();
  if ((unsigned long)(currentMillis - previousMillisDisplay) >= intervalDisplay) {
    previousMillisDisplay = millis();

    Serial.print("SHUNT,"); Serial.print(shunt.getVoltage());
    Serial.print(",");      Serial.print(shunt.getCurrent());
    Serial.print(",");      Serial.print(shunt.getAh());
    Serial.print(",");      Serial.print(shunt.getWh());
    Serial.print(",");      Serial.print(shunt.getOverflow());
    Serial.println();
  }
  delay(10);
}

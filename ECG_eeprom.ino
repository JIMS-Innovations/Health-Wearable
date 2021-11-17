#include "EEPROM.h"
int addr = 0;
#define EEPROM_SIZE 128



void setup() {
 Serial.begin(115200);
  Serial.println("start...");
  if (!EEPROM.begin(EEPROM_SIZE))
  {
    Serial.println("failed to initialise EEPROM"); delay(1000000);
  }
  Serial.println(" bytes read from Flash . Values are:");
  for (int i = 0; i < EEPROM_SIZE; i++)
  {
    Serial.print(byte(EEPROM.read(i))); Serial.print(" ");
  }
  Serial.println();
  Serial.println("writing random n. in memory");

}

void loop() {
  // put your main code here, to run repeatedly:

}

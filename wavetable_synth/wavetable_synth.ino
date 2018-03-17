#include <LiquidCrystal.h>
#include <math.h>
LiquidCrystal LCDDriver(11,9,5,6,7,8);

unsigned long Timer;
int count;

void setup() {
  // put your setup code here, to run once:
  LCDDriver.begin(16,2);
  LCDDriver.clear();
  count = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - Timer >= 1000) { 
    LCDDriver.setCursor(0,0);
    LCDDriver.print(count);
    count += 1;
    Timer += 1000;
  }
}

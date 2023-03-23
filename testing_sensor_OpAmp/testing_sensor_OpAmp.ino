#define Vin A7
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const float scaleFactor = 5.0/1023.0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  pinMode(Vin, INPUT);

}

long int count = 0;
void loop() {
  // put your main code here, to run repeatedly:
  float raw = analogRead(Vin);
  float voltage = raw*scaleFactor;

  Serial.print("Count: ");
  Serial.println(count++);
  Serial.print("Raw: ");
  Serial.println(raw);
  Serial.print("Voltage: ");
  Serial.println(voltage);
  Serial.println();
  delay(500);

}

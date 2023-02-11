/*
Temperature PID Controller using a KY-40 rotary encoder.

Temperature is assumed to be in Celcius, unless otherwise stated.

Written by Thomas Bourgeois.

*/


#include <LiquidCrystal_I2C.h>
#include <BfButton.h>  // For Encoder.
#include <PID_v1.h>    // PID Controller library.

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ***************************************************** Defining Pins *****************************************************

// pins for encoder
#define CLK 2
#define DT 3
#define SW 4
BfButton btn(BfButton::STANDALONE_DIGITAL, SW, true, LOW);

// pins for RBG LED
#define GREEN 6
#define RED 7


// ***************************************************** Variables *****************************************************
const float temperature_range[] = { 35, 150 };  // allowable temperature range.

// Variables for Encoder:
const float increment = 0.5;  // increment for each turn
const float holdTime = 1000;  // how long to hold the button down.

// Variables for heater
bool powerState = false;

// PID Variables
double Setpoint = temperature_range[0];                            // user set temperature.
double cur_temperature;                                            // current temperature
double Output;                                                     // output to heater.
PID myPID(&cur_temperature, &Output, &Setpoint, 1, 1, 1, DIRECT);  //Specify the links and initial tuning parameters


// Need for rotary Encoder
// DO NOT CHANGE THE FOLLOWING
int preCLK;  // previous states
int preDATA;
long TimeOfLastDebounce = 0;  // variables for debouncing.
const long DelayofDebounce = 0.01;

// ***************************************************** Functions *****************************************************

void green_led(bool state) {
  // Function to turn on/off green led.
  digitalWrite(GREEN, state);
}

void red_led(bool state) {
  // Function to turn on/off red led.
  digitalWrite(RED, state);
}


void read_button(BfButton *btn, BfButton::press_pattern_t pattern) {
  // Function to read button on the Encoder.

  switch (pattern) {

    case BfButton::SINGLE_PRESS:
      Setpoint = temperature_range[0];
      updateDisplay();
      break;

    case BfButton::LONG_PRESS:
      if (powerState) {
        powerState = false;
        green_led(false);
        red_led(true);
      } else {
        green_led(true);
        red_led(false);
        powerState = true;
      }
      break;
  }
}


void updateDisplay() {
  // Function to update display

  Serial.print("Set Temperature: ");
  Serial.println(Setpoint);
  Serial.print("C Temperature: ");
  Serial.println(Setpoint, 1);

  lcd.setCursor(0, 0);
  lcd.print("Set Temp: ");
  lcd.print(Setpoint, 1);
  lcd.print((char)223);
  lcd.print("C");

  lcd.setCursor(0, 1);
  lcd.print("Cur Temp: ");
  lcd.print(Setpoint, 1);
  lcd.print((char)223);
  lcd.print("C");
}


void check_rotary() {

  if ((preCLK == 0) && (preDATA == 1)) {
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 0)) {
      Setpoint += increment;
      Serial.println(Setpoint);
    }
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      Setpoint -= increment;
      Serial.println(Setpoint);
    }
  }

  if ((preCLK == 1) && (preDATA == 0)) {
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 1)) {
      Setpoint += increment;
      Serial.println(Setpoint);
    }
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 0)) {
      Setpoint -= increment;
      Serial.println(Setpoint);
    }
  }

  if ((preCLK == 1) && (preDATA == 1)) {
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 1)) {
      Setpoint += increment;
      Serial.println(Setpoint);
    }
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 0)) {
      Setpoint -= increment;
      Serial.println(Setpoint);
    }
  }

  if ((preCLK == 0) && (preDATA == 0)) {
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      Setpoint += increment;
      Serial.println(Setpoint);
    }
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      Setpoint -= increment;
      Serial.println(Setpoint);
    }
  }
}

// ***************************************************** Main Program *****************************************************

void setup() {
  // put your setup code here, to run once:
  lcd.init();  // initialize the lcd
  lcd.backlight();

  Serial.begin(9600);
  updateDisplay();

  // pinMode(CLK, INPUT);
  // pinMode(DT, INPUT);
  // pinMode(SW, INPUT_PULLUP);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  red_led(true);  // initial heater is turned off.

  btn.onPress(read_button)  // single click
    // .onDoublePress(read_button) // double click
    .onPressFor(read_button, holdTime);  // hold for 1sec for turning on/off heating.

  // Read the initial state of Encoder.
  preCLK = digitalRead(CLK);
  preDATA = digitalRead(DT);
}


void loop() {

  // ************ Encoder stuff ************
  btn.read();
  if ((millis() - TimeOfLastDebounce) > DelayofDebounce) {
    check_rotary();
    preCLK = digitalRead(CLK);
    preDATA = digitalRead(DT);
    TimeOfLastDebounce = millis();
    updateDisplay();
  }
  // ************************************************
  // Future code here...
}
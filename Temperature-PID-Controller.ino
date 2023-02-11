/*
Temperature PID Controller  using an rotary encoder.

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

// Variables for Encoder
int currentStateCLK;
int lastState;
int curState;
const double increment = 0.25;

// Variables for heater
bool powerState = false;

// PID Variables
double Setpoint = temperature_range[0];                            // user set temperature.
double cur_temperature;                                            // current temperature
double Output;                                                     // output to heater.
PID myPID(&cur_temperature, &Output, &Setpoint, 1, 1, 1, DIRECT);  //Specify the links and initial tuning parameters


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

// ***************************************************** Main Program *****************************************************

void setup() {
  // put your setup code here, to run once:
  lcd.init();  // initialize the lcd
  lcd.backlight();

  Serial.begin(9600);
  updateDisplay();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  btn.onPress(read_button)  // single click
    // .onDoublePress(read_button) // double click
    .onPressFor(read_button, 1000);  // hold for 1sec for turning on/off heating.

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  red_led(true); // initial heater is turned off.

  // Read the initial state of Encoder.
  lastState = digitalRead(CLK);
}


void loop() {

  // ************ Encoder stuff ************
  btn.read();
  int curState = digitalRead(CLK);
  if (curState != lastState) {
    if (digitalRead(DT) != curState) {  // clockwise
      Setpoint += increment;
    } else {  // counter-clockwise
      Setpoint -= increment;
    }
    // if user goes beyond temperature allowable range.
    if (Setpoint > temperature_range[1]) {
      Setpoint = temperature_range[1];
    } else if (Setpoint < temperature_range[0]) {
      Setpoint = temperature_range[0];
    }
    updateDisplay();
  }
  lastState = curState;
  // ************************************************
  // Future code here...
}
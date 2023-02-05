/*

Temperature PID Controller Interface using an rotary encoder.


Written by Thomas Bourgeois.
*/




#include <BfButton.h>


// ***************************************************** Defining Pins *****************************************************

// pins for encoder
#define CLK 2
#define DT 3
#define SW 4
BfButton btn(BfButton::STANDALONE_DIGITAL, SW, true, LOW);

// pins for RBG LED
#define RED A0
#define GREEN A1
#define BLUE A2

// ***************************************************** Variables *****************************************************

// Variables for Encoder
int currentStateCLK;
int lastState;
int curState;

// Temperature variables
const float base_temperature = 35;
const float max_temperature = 100;
float set_temperature = base_temperature;

// Variables for heater
bool powerState = false;

// ***************************************************** Functions *****************************************************

void led(int red, int green, int blue) {
  /*
    Function to color the rbg led

    Parameters:
      red, green, blue (int): scaled from 0 to 100.
  */

  red = map(red, 0, 100, 0, 255);
  green = map(green, 0, 100, 0, 255);
  blue = map(blue, 0, 100, 0, 255);


  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}


void read_button(BfButton *btn, BfButton::press_pattern_t pattern) {
  // Function to read button on the Encoder.

  switch (pattern) {

    case BfButton::SINGLE_PRESS:
      set_temperature = base_temperature;
      updateDisplay();
      break;

    case BfButton::LONG_PRESS:
      if (powerState) {
        powerState = false;
        led(0, 0, 0);
      } else {
        powerState = true;
        led(0, 100, 0);
      }
      break;
  }
}

void updateDisplay() {
  // Function to update display

  Serial.print("Set Temperature: ");
  Serial.println(set_temperature);
}


// ***************************************************** Setup *****************************************************

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  updateDisplay();

  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  btn.onPress(read_button)  // single click
    // .onDoublePress(read_button) // double click
    .onPressFor(read_button, 1000);  // hold for 1sec for turning on/off heating.

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  // Read the initial state of Encoder.
  lastState = digitalRead(CLK);
}

// ***************************************************** Main Loop *****************************************************

void loop() {
  // put your main code here, to run repeatedly:

  //Encoder rotation tracking
  btn.read();
  int curState = digitalRead(CLK);
  if (curState != lastState) {
    if (digitalRead(DT) != curState) {  // clockwise
      set_temperature += 0.5;
    } else {  // counter-clockwise
      set_temperature -= 0.5;
    }

    // conditions if user goes beyond temperature allowable range.
    if (set_temperature > max_temperature) {
      set_temperature = max_temperature;
    } else if (set_temperature < base_temperature) {
      set_temperature = base_temperature;
    }
    updateDisplay();
  }
  lastState = curState;

  // Future code here...
}
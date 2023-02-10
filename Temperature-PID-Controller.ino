/*
Temperature PID Controller  using an rotary encoder.

Written by Thomas Bourgeois.

*/
#include <LiquidCrystal_I2C.h>
#include <BfButton.h> // For Encoder.
#include <PID_v1.h> // PID Controller library.

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ***************************************************** Variables *****************************************************
const float base_temperature = 35;
const float max_temperature = 100;

// Variables for Encoder
int currentStateCLK;
int lastState;
int curState;
const double increment = 0.25;



// Variables for heater
bool powerState = false;

// PID Variables
double Setpoint = base_temperature;
double cur_temperature;
double Output;


//Specify the links and initial tuning parameters
PID myPID(&cur_temperature, &Output, &Setpoint, 2,5,1, DIRECT);


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


// ***************************************************** Functions *****************************************************

void led(int red, int green, int blue) {
  /*
    Function to color the rbg led

    Parameters:
      red, green, blue (int 0 - 255)
  */
  analogWrite(RED, red);
  analogWrite(GREEN, green);
  analogWrite(BLUE, blue);
}


void read_button(BfButton *btn, BfButton::press_pattern_t pattern) {
  // Function to read button on the Encoder.

  switch (pattern) {

    case BfButton::SINGLE_PRESS:
      Setpoint = base_temperature;
      updateDisplay();
      break;

    case BfButton::LONG_PRESS:
      if (powerState) {
        powerState = false;
        led(0, 0, 0);
      } else {
        powerState = true;
        led(0, 255, 0);
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
  lcd.init(); // initialize the lcd
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
  pinMode(BLUE, OUTPUT);

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
    if (Setpoint > max_temperature) {
      Setpoint = max_temperature;
    } else if (Setpoint < base_temperature) {
      Setpoint = base_temperature;
    }
    updateDisplay();
  }
  lastState = curState;
  // ************************************************
  // Future code here...


  
}
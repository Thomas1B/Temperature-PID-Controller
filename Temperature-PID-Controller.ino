/*
Temperature PID Controller using a KY-40 rotary encoder.

Temperature is assumed to be in Celcius, unless otherwise stated.

Written by Thomas Bourgeois.

*/


#include <LiquidCrystal_I2C.h>
#include <BfButton.h>  // For Encoder.
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <PID_v1.h>  // PID Controller library.


// ***************************************************** Defining Pins *****************************************************

// pins for Rotary Encoder
#define CLK 6
#define DT 7
#define SW 8
BfButton btn(BfButton::STANDALONE_DIGITAL, SW, true, LOW);  // defining Encoder button object

// pins for LEDs
#define RED_LED 2
#define GREEN_LED 3

#define LM19_pin A7  // Reference voltage pin

// Declaring the OLED object.
// Data pin on arduino UNO/NANO: A4(SDA), A5(SCL)
#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);


// ***************************************************** Variables *****************************************************

const float Vref = 4.68;                        // Reference voltage lm19 is using.
const float temperature_range[] = { 25, 200 };  // allowable temperature range.

// Variables for Encoder:
float increment = 0.5;        // increment for each turn.
const float holdTime = 1000;  // how long to hold the button down.

// Variables for heater
bool powerState = false;

// PID Variables
double const start_Setpoint = 100.0;                               // user set temperature.
PID myPID(&cur_temperature, &Output, &Setpoint, 1, 1, 1, DIRECT);  // Specify the links and initial tuning parameters

const long DelayofTempRead = 2000;  // milliseconds.

// ************************ DO NOT CHNAGE THESE VARIABLES ************************

double Setpoint = start_Setpoint;  // user set temperature.
double cur_temperature;            // current temperature
double Output;                     // output to heater.

// Need for rotary Encoder
int preCLK;  // previous states
int preDT;
long TimeOfLastDebounce = 0;  // variables for debouncing.
const long DelayofDebounce = 0.01;

// delay time for reading temperaturez
long TimeOfLastTempRead = 0;
// ***************************************************** Main Program *****************************************************
void (*resetFunc)(void) = 0;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);  // Serial to print messages.

  /* Defining pinModes */
  pinMode(CLK, INPUT);  // Encoder
  pinMode(DT, INPUT);
  pinMode(SW, INPUT_PULLUP);

  pinMode(LM19_pin, INPUT);  // LM19 Temp Sensor

  pinMode(RED_LED, OUTPUT);  // LEDs
  pinMode(GREEN_LED, OUTPUT);

  btn.onPress(read_encoder_btn)               // single click
    .onDoublePress(read_encoder_btn)          // double click
    .onPressFor(read_encoder_btn, holdTime);  // hold for 1sec for turning on/off heating.

  // Read the initial state of Encoder.
  preCLK = digitalRead(CLK);
  preDT = digitalRead(DT);

  set_up_OLED();      // OLED screen for displaying information.
  RED_LED_led(true);  // initial heater is turned off.
}



void loop() {

  // ************ Encoder stuff ************
  btn.read();
  if ((millis() - TimeOfLastDebounce) >= DelayofDebounce) {
    check_rotary();
    preCLK = digitalRead(CLK);
    preDT = digitalRead(DT);
    TimeOfLastDebounce = millis();
  }

  if ((millis() - TimeOfLastTempRead) >= DelayofTempRead) {
    TimeOfLastTempRead = millis();
    cur_temperature = get_temperature(LM19_pin);
    updateInfo();
  }
}

// ***************************************************** Functions *****************************************************



void GREEN_LED_led(bool state) {
  // Function to turn on/off GREEN_LED led.
  digitalWrite(GREEN_LED, state);
}

void RED_LED_led(bool state) {
  // Function to turn on/off RED_LED led.
  digitalWrite(RED_LED, state);
}


void read_encoder_btn(BfButton* btn, BfButton::press_pattern_t pattern) {
  /*
    Function to handle the button on the Encoder.

      Parameters:
        btn: BfButton object.
        pattern: click pattern

  */

  switch (pattern) {

    case BfButton::SINGLE_PRESS:
      Setpoint = start_Setpoint;
      updateInfo();
      break;

    case BfButton::DOUBLE_PRESS:
      if (increment == 0.5) {
        increment = 10.0;
      } else {
        increment = 0.5;
      }
      break;

    case BfButton::LONG_PRESS:
      if (powerState) {
        powerState = false;
        GREEN_LED_led(false);
        RED_LED_led(true);
      } else {
        GREEN_LED_led(true);
        RED_LED_led(false);
        powerState = true;
      }
      break;
  }
}

void check_rotary() {

  float dummy_temp = Setpoint;
  float var_Setpoint = Setpoint;  // temporary variable for allowable temperature range

  if ((preCLK == 0) && (preDT == 1)) {
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 0)) {
      var_Setpoint += increment;
    }
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      var_Setpoint -= increment;
    }
  }

  if ((preCLK == 1) && (preDT == 0)) {
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 1)) {
      var_Setpoint += increment;
    }
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 0)) {
      var_Setpoint -= increment;
    }
  }

  if ((preCLK == 1) && (preDT == 1)) {
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 1)) {
      var_Setpoint += increment;
    }
    if ((digitalRead(CLK) == 0) && (digitalRead(DT) == 0)) {
      var_Setpoint -= increment;
    }
  }

  if ((preCLK == 0) && (preDT == 0)) {
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      var_Setpoint += increment;
    }
    if ((digitalRead(CLK) == 1) && (digitalRead(DT) == 1)) {
      var_Setpoint -= increment;
    }
  }

  if (var_Setpoint != Setpoint) {
    if (var_Setpoint > temperature_range[1]) {
      var_Setpoint = temperature_range[1];
    } else if (var_Setpoint < temperature_range[0]) {
      var_Setpoint = temperature_range[0];
    }

    Setpoint = var_Setpoint;
    updateInfo();
  }
}

void OLED_msg(String text, boolean clear_display = true) {
  /*
    Function to display a message on the OLED Screen.
  */

  if (clear_display) {
    OLED_clear();
  };
  display.println(text);
  display.display();
}

void OLED_clear() {
  // Function to clear the OLED screen.
  display.clearDisplay();
  display.display();
  display.setCursor(0, 0);
}

void set_up_OLED() {
  /* 
    Function to set up the OLED and check if connection successful.
  */

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("Failed to connect to OLED...");
    while (true) {
      continue;
    }
  } else {
    // Setting default text size and color
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.clearDisplay();

    // Displaying startup messege
    display.write("Starting");
    display.write("\nPID");
    display.write("\nController");
    display.display();
    delay(5000);
    OLED_clear();
  }
}


double get_temperature(int pin) {
  /*
  Function to convert the output voltage of a LM19 Sensor to temperature in Celsius.

  Parameter:
    Pin - int: analog pin to LM19 Sensor.

  Returns:
    temperature as a double.

  See https://www.ti.com/lit/ds/symlink/lm19.pdf, page 2 for the equation.
  */

  double Vo = analogRead(pin);
  Vo = Vo * Vref / 1023.0;  // mapping value to a output voltage.

  // // Constants to simplify the equation
  double c1 = -1481.96;
  double c2 = 2.1962e6;
  double c3 = 1.8639;
  double c4 = 3.88e-6;

  return c1 + sqrt(c2 + ((c3 - Vo) / c4));
}

String double_to_string(double value, int decimal_places = 2) {
  /* 
  Function to convert a double to a String

    Parameters:
      value: double number to convert.
      round: what decimal place to round to
  */
  char buffer[10];
  dtostrf(value, 4, decimal_places, buffer);
  return String(buffer);
}


void updateInfo() {
  /*
  Function to update the display with temperature and setpoint.
  */
  display.clearDisplay();
  display.setCursor(0, 0);

  String text = "";

  // updating temperature
  if (cur_temperature > 0) {
    text = "T: " + double_to_string(cur_temperature, 1);
    display.print(text);
  } else {
    display.print("T: < 0");
  }
  display.write(247);
  display.print("C");

  // updating setpoint
  text = "\n\nSet T:\n" + double_to_string(Setpoint, 1);
  display.print(text);
  display.write(247);
  display.print("C");
  display.display();
}

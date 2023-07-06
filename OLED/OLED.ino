// Script to test OLED Screen

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 32  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO/NANO:       A4(SDA), A5(SCL)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);



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
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;  // Don't proceed, loop forever
  } else {
    Serial.println("SSD1306 Connection Successful!");  // successful msg to terminal
    // Setting default text size and color
    display.setTextSize(1);
    display.setTextColor(WHITE);

    // Displaying welcome messege
    display.clearDisplay();
    // display.print("Welcome to the\n");
    // display.print("Temperature\nPID Controller!");
    // String text = String("Welcome to the\nTemperature\nPID Controller!");
    char* text = "Welcome to the\nTemperature\nPID Controller!";
    OLED_msg(text);
    delay(5000);
    OLED_clear();
  }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  set_up_OLED();
}

void loop() {
  // put your main code here, to run repeatedly:
}

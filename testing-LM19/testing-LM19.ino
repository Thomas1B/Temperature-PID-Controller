/*
Testing the a Temperature Sensor

LM19 Datasheet: https://www.ti.com/lit/ds/symlink/lm19.pdf
*/



// ****** Defining pins ******

#define sensor_pin A0



// ******* Program set up *******
void setup() {
  // put your setup code here, to run once:
  pinMode(sensor_pin, INPUT);
  Serial.begin(9600);
  Serial.println("\nProgram Starting...\n");
}


// ******* main program *******

int counter = 1;
void loop() {

  double val = analogRead(sensor_pin);

  Serial.print("Counter: ");
  Serial.println(counter);
  counter++;

  Serial.print("Raw Value: ");
  Serial.println(val);
  Serial.print("Voltage = ");
  Serial.println(val * 5 / 1023);

  double temp = get_temperature(sensor_pin);
  Serial.print("Temperature: ");
  Serial.println(temp);

  Serial.println();
  delay(2000);
}

// ******* Functoins *******

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
  Vo = Vo * 5 / 1023;  // mapping value to a output voltage.

  // Constants to simplify the equation
  double c1 = -1481.96;
  double c2 = 2.1962e6;
  double c3 = 1.8639;
  double c4 = 3.88e-6;

  double temperature = c1 + sqrt(c2 + ((c3 - Vo) / c4));
  // temperature = (temperature - 32) * 5 / 9; # converting to celsius.

  return temperature;
}

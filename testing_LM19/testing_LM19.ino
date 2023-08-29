#define LM19_pin A7  // Reference voltage pin
// See https://www.ti.com/lit/ds/symlink/lm19.pdf for equations.

// delay time for reading temperaturez
long TimeOfLastTempRead = 0;
const long DelayofTempRead = 250;  // milliseconds.

const float Vref = 4.68; // refence for voltage for sensor.
const float temperature_range[] = { 25, 200 };  // allowable temperature range.



void setup() {
  Serial.begin(9600);  // Serial to print messages.

  pinMode(LM19_pin, INPUT);  // LM19 Temp Sensor
  delay(2000);
  Serial.print("voltage");
  // Serial.print(" temperature");
  Serial.println();
}

void loop() {
  // put your main code here, to run repeatedly:
  if ((millis() - TimeOfLastTempRead) >= DelayofTempRead) {
    TimeOfLastTempRead = millis();
    double Vo = analogRead(LM19_pin);
    Vo = Vo * Vref / 1023.0;  // mapping value to a output voltage.
    // Vo = Vo - 0.135;

    // double temperature = linear(Vo);
    // double temperature = adj_linear(Vo);
    // double temperature = parabolic(Vo);

    Serial.print(Vo, 3);
    // Serial.print(" ");
    // Serial.print(temperature);
    Serial.println();
  }
}

double parab_transfer_function(double temp) {
  /*
  Function to calculate the voltage using the parabolic transfer function

    See https://www.ti.com/lit/ds/symlink/lm19.pdf
  */
  return -3.88e-6 * pow(temp, 2) - 1.15e-2 * temp + 1.8639;  // units: V
}



double linear(double Vo) {
  // Function to calculate temperature using the linear function.

  return (Vo - 1.8663) / -11.69e-3;  // units: C
}

double adj_linear(double Vo) {
  // Function to calculate temperature using the adjusted linear function.

  double T_mid = (temperature_range[0] + temperature_range[1]) / 2;                      // units: C
  double m = -7.76e-6 * T_mid - 0.0115;                                                  // units: V/C
  double Vop_max = parab_transfer_function(temperature_range[1]);                        // units: V
  double Vop = parab_transfer_function(T_mid);                                           // units: V
  double adjusted_intercept = Vop_max + Vop - m * ((temperature_range[1] + T_mid) / 2);  // units: V

  return (Vo - adjusted_intercept) / m;  // units: C
}


double parabolic(double Vo) {
  // Function to calculate temperature using the parabolic function

  // // Constants to simplify the equation
  double c1 = -1481.96;
  double c2 = 2.1962e6;
  double c3 = 1.8639;
  double c4 = 3.88e-6;

  return c1 + sqrt(c2 + ((c3 - Vo) / c4));
}

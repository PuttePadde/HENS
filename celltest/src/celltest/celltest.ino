/**
 * Complete project details at https://RandomNerdTutorials.com/arduino-load-cell-hx711/.
 *
 * HX711 library for Arduino - example file.
 * https://github.com/bogde/HX711.
 *
 * MIT License.
 * (c) 2018 Bogdan Necula.
 *
**/

#include <Arduino.h>
#include "HX711.h"

// HX711 circuit wiring.
const int LOADCELL_DOUT_PIN = 2;
const int LOADCELL_SCK_PIN = 3;

HX711 scale;

void setup() {
  Serial.begin(57600);
  Serial.println("HX711 Demo");
  Serial.println("Initializing the scale");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN, 128);

  Serial.println("Before setting up the scale:");
  Serial.print("read: \t\t");
  Serial.println(scale.read());      // Print a raw reading from the ADC.

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(20));   // Print the average of 20 readings from the ADC.

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // Print the average of 5 readings from the ADC minus the tare weight (not set yet).

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);  // Print the average of 5 readings from the ADC minus tare weight (not set) divided.
            // By the SCALE parameter (not set yet).
            
  scale.set_scale(-90.92);
  //scale.set_scale(-471.497);                      // This value is obtained by calibrating the scale with known weights; see the README for details.
  scale.tare();               // Reset the scale to 0.

  Serial.println("After setting up the scale:");

  Serial.print("read: \t\t");
  Serial.println(scale.read());                 // Print a raw reading from the ADC.

  Serial.print("read average: \t\t");
  Serial.println(scale.read_average(100));       // Print the average of 20 readings from the ADC.

  Serial.print("get value: \t\t");
  Serial.println(scale.get_value(5));   // Print the average of 5 readings from the ADC minus the tare weight, set with tare().

  Serial.print("get units: \t\t");
  Serial.println(scale.get_units(5), 1);        // Print the average of 5 readings from the ADC minus tare weight, divided.
            // By the SCALE parameter set with set_scale.

  Serial.println("Readings:");
}

void loop() {
  float reading = scale.get_units();      // Single reading.
  float average = scale.get_units(100);    // Average of 10 readings.
  
  // Print values in a format compatible with the Serial Plotter.
  Serial.print(reading, 1);
  Serial.print("\t");                     // Tab separates values for plotting.
  Serial.println(average, 5);             // Newline ends the line for plotting.

  delay(5000);                            // Delay to control update rate.
}

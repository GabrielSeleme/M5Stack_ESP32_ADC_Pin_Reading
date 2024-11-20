/*
 * Project: ADC Reader for M5Stack ESP32
 * Author: Gabriel Seleme
 * Date: October, 2024
 * Description: Real time reading of a ADC pin value from the M5Stack ESP32
 * Platform: M5Stack Core, PlatformIO, Visual Studio Code
 * Required Libraries:
 * - M5Stack library (https://github.com/m5stack/M5Stack)
 */

#include <M5Stack.h>

#define ADC_PIN 36 // ADC pin GPIO36.

int time_reading = 0; // create a variable to count time.
int voltage_reading_old = 0; // create a variable to store the last readed value.
int time_reading_old = 0; // create a variable to store the last time value.

// The following variables will specify the width and height (X & Y axis) of the graph. Values are chosen according with the Lcd's size.
int axisX_i = 40;
int axisY_i = 40;
int axisX_f = 310;
int axisY_f = 220;

int scaleY = 60; // this will be later used to adjuste the scale of the readed values

void graphSetup(int axisX_i, int axisY_i, int axisX_f, int axisY_f); // This function will be used to create the axis lines, axis scale, axis titles and grid lines. 

void setup() {
  M5.begin(); // Start the microcontroller.
  analogReadResolution(10); // Specify the reading resolution, not sure wether this works or not for this microcontroller.
  graphSetup(axisX_i, axisY_i, axisX_f, axisY_f); // Create the axis lines, scale, titles and grid lines for the graph to be plot on.
  Serial.begin(115200); // Set the data rate in bits/s for the serial data transmission.
}

void loop() {
  int adcValue = analogRead(ADC_PIN); // Create a variable to receive the current value of the stablished ADC pin.

  if(adcValue >= scaleY){ // If the readed value is higher than the highest expected value to be read it adjustes the scale.
    axisY_f = scaleY + 20;
  }

  int voltage_reading = map(adcValue, 0, scaleY, axisY_f, axisY_i); // Set an appropriate scale for the readed value according to the specified width and height of the graph.

  M5.Lcd.setTextSize(4); // Set text size.
  M5.Lcd.setCursor(10,4); // Set text position on the screen.
  M5.Lcd.printf("%4d", adcValue); // Print the readed value on the screen.

  M5.Lcd.drawLine(time_reading_old + axisX_i, voltage_reading_old, time_reading + axisX_i, voltage_reading, TFT_WHITE);
  /*the above command draw a line between two points (x0, y0) and (x1, y1), where:
  x0 = last time value
  y0 = last readed value
  x1 = current time value
  y1 = current readed value

  If one desires to print dots instead of lines the command below can be used alternatively:
  M5.Lcd.drawPixel(time_reading + axisX_i, voltage_reading, TFT_WHITE); // Draw a pixel in the position (x, y) = (current time, current readed value)
  */

  voltage_reading_old = voltage_reading; // Store the last readed value to be the old in the next run.
  time_reading_old = time_reading; // Store the last time value to be the old in the next run.
  time_reading++; // Sums +1 on the time variable to go to the next time/axis X position.

  if(time_reading >= axisX_f - axisX_i){ // Check wether the current time value is bigger than the last possible position on the X axis.
    time_reading = 0; // If the condition stablished above is true then it erases the current graph and start another one from X = 0.
    time_reading_old = 0; // Stores 0 on the old X position value.
    voltage_reading_old = 0; // Stores 0 on the old Y position value.
    scaleY = 60; // Reset the scale to the one first stablished.
    graphSetup(axisX_i, axisY_i, axisX_f, axisY_f); // Setup the graph again (axis lines, grid lines etc).
  }
  delay(100); // Delay 0.1 s from the last reading to start another.
}

void graphSetup(int axisX_i, int axisY_i, int axisX_f, int axisY_f){
  M5.Lcd.clear(DARKGREEN); // Clean what was being previously shown in the screen and set it with a dark green background.
  M5.Lcd.setTextColor(TFT_WHITE, DARKGREEN);
  M5.Lcd.setTextSize(2);

  M5.Lcd.setRotation(0); // Rotate the reference in the Lcd so the Y axis title is written vertical. Next 2 commands position it on the Lcd.
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.drawString("Voltage", axisX_f/2, (axisY_i)/2);
  
  M5.Lcd.setRotation(1); // Rotate the reference to the standard one so the X axis title is written horizontal. Next 2 commands position it on the Lcd.
  M5.Lcd.setTextDatum(MC_DATUM); // Write the title of the X axis
  M5.Lcd.drawString("Time (s)", (axisX_i + axisX_f) / 2, axisY_f + 15);

  M5.Lcd.drawLine(axisX_i,axisY_i, axisX_i, axisY_f, TFT_WHITE); // draw the X axis of the graph.
  M5.Lcd.drawLine(axisX_i,axisY_f, axisX_f, axisY_f, TFT_WHITE); // draw the Y axis of the graph.

  for(int i = 50; i <= axisX_f - axisX_i; i += 50){ // This loop will draw vertical grid lines for each desired position in the X axis (in this case to every 5 s).
      M5.Lcd.drawLine(axisX_i + i, axisY_i, axisX_i + i, axisY_f, TFT_GREEN); // Draw the grid line.
      M5.Lcd.setTextSize(2); // Set text size.
      M5.Lcd.setCursor(i + axisX_i,axisY_f - 10); // Set text position on the screen.
      M5.Lcd.printf("%d", i/10); // Print the X value next to the grid line.
  }

  for(int i = 30; i <= axisY_f - axisY_i; i += 30){ //This loop will draw horizontal grid lines for each desired position in the Y axis.
      M5.Lcd.drawLine(axisX_i, axisY_f - i, axisX_f, axisY_f - i, TFT_GREEN); // Draw the grid line.
      M5.Lcd.setTextSize(2); // Set text size.
      M5.Lcd.setCursor(axisX_i - 10, axisY_f - i); // Set text position on the screen.
      M5.Lcd.printf("%d", i/3 ); // Print the Y value next to the grid line.
  }
}
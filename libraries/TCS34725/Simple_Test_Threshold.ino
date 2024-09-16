#include <Wire.h>
#include "Adafruit_TCS34725.h"

#define redpin 3
#define greenpin 15
#define yellowpin 13
#define commonAnode true

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

const int WINDOW_SIZE = 2000 / 500;  // Assuming loop delay is 500ms, for a window size of 2 seconds.
float averageWindow[WINDOW_SIZE] = {0};
int windowIndex = 0;
const int THRESHOLD_Y = 200;

void setup() {
  Serial.begin(9600);
  
  if (!tcs.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    while (1);
  }

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(yellowpin, OUTPUT);
}

void flash(int delayDuration) {
  digitalWrite(LED_BUILTIN, LOW);
  delay(delayDuration);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(delayDuration);
}

void loop() {
  float red, green, blue;
  
  tcs.getRGB(&red, &green, &blue);
  float average = (red + green + blue) / 3;

  // Handle sliding window
  averageWindow[windowIndex] = average;
  windowIndex = (windowIndex + 1) % WINDOW_SIZE;
  float sum = 0;
  for(int i = 0; i < WINDOW_SIZE; i++) {
    sum += averageWindow[i];
  }
  float windowAverage = sum / WINDOW_SIZE;

  // Decide to flash onboard LED based on window average and threshold
  if(windowAverage > THRESHOLD_Y) {
    int delayDuration = map(windowAverage, THRESHOLD_Y, 255, 500, 50);  // As the reading goes higher, delay gets shorter (faster flash)
    flash(delayDuration);
  }

  delay(500);  // Assuming a 500ms delay for each loop iteration. Adjust as needed.
}

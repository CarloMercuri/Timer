#include "LEDController.h"
#include "Arduino.h"

/**
 * Constructor for the LEDController class.
 * Initializes the pins to be used for the red, green and blue colors
 *
 * @param redPin: Digital pin number on the Arduino board to which the red pin is connected.
 * @param yellowPin: Digital pin number on the Arduino board to which the yellow pin is connected.
 * @param greenPin: Digital pin number on the Arduino board to which the green pin is connected.
 */

LEDController::LEDController(int r_pin, int g_pin, int b_pin) {
  is_on = false;
  pin_r = r_pin;
  pin_g = g_pin;
  pin_b = b_pin;
  pinMode(pin_r, OUTPUT);
  pinMode(pin_g, OUTPUT);
  pinMode(pin_b, OUTPUT);
}

void LEDController::SetColor(int red, int green, int blue) {
  r = red;
  g = green;
  b = blue;

  if (is_on) {
    analogWrite(pin_r, r);
    analogWrite(pin_g, g);
    analogWrite(pin_b, b);
  }
}

void LEDController::SwitchOn() {
  is_on = true;
  analogWrite(pin_r, r);
  analogWrite(pin_g, g);
  analogWrite(pin_b, b);
}

void LEDController::SwitchOff() {
  is_on = false;
  analogWrite(pin_r, 0);
  analogWrite(pin_g, 0);
  analogWrite(pin_b, 0);
}

void LEDController::YellowBlink() {
  for (int i = 0; i < 10; i++) {
    SwitchOff();
    delay(200);
    SwitchOn();
    Serial.println("blink");
    SetColor(70, 20, 0);
    delay(200);
  }
  SetColor(0, 0, 0);
}

void LEDController::RedBlink() {
  for (int i = 0; i < 10; i++) {
    SwitchOff();
    delay(200);
    SwitchOn();
    Serial.println("blink");
    SetColor(70, 0, 0);
    delay(200);
  }
  SetColor(0, 0, 0);
}


#include "ButtonController.h"
#include "Arduino.h"
#include "NetComm.h"
NetComm _comm2;
/**
 * Constructor for the ButtonController class.
 *
 * @param yellowBPin: Digital pin number on the Arduino board to which the yellow button pin is connected.
 * @param whiteBPin: Digital pin number on the Arduino board to which the white pin button is connected.
 * @param redBPin: Digital pin number on the Arduino board to which the red button pin is connected.
 * @param blueBPin: Digital pin number on the Arduino board to which the blue button pin is connected.
 */

ButtonController::ButtonController(int yB_pin, int wB_pin, int rB_pin, int bB_pin)
{
  pin_yB = yB_pin;
  pin_wB = wB_pin;
  pin_rB = rB_pin;
  pin_bB = bB_pin;
  pinMode(pin_yB, INPUT_PULLUP);
  pinMode(pin_wB, INPUT_PULLUP);
  pinMode(pin_rB, INPUT_PULLUP);
  pinMode(pin_bB, INPUT_PULLUP);
}

void ButtonController::Interrupt(int _buttonId)
{
   buttonId = _buttonId;
   unixTimestamp = 1725445282;
   Serial.println("Connected to wifi - start send event");
   _comm2.SendButtonEventRequest(buttonId, unixTimestamp);
}

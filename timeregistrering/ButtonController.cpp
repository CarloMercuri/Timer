
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

ButtonController::ButtonController(DataStorage* data_storage, int b1_pin, int b2_pin, int b3_pin, int b4_pin)
{
  _buttons[0].button_id = 1;
  _buttons[0].pin = b1_pin;
  _buttons[0].is_handled = false;
  _buttons[0].is_pressed = false;
  pinMode(b1_pin, INPUT);

   _buttons[1].button_id = 2;
  _buttons[1].pin = b2_pin;
  _buttons[1].is_handled = false;
  _buttons[1].is_pressed = false;
  pinMode(b2_pin, INPUT);

   _buttons[2].button_id = 3;
  _buttons[2].pin = b3_pin;
  _buttons[2].is_handled = false;
  _buttons[2].is_pressed = false;
  pinMode(b3_pin, INPUT);

   _buttons[3].button_id = 4;
  _buttons[3].pin = b4_pin;
  _buttons[3].is_handled = false;
  _buttons[3].is_pressed = false;
  pinMode(b4_pin, INPUT);

  _data = data_storage;  
}

void ButtonController::CheckInputs(){
 for (int i = 0; i < 4; i++) {
    bool is_down = (digitalRead(_buttons[i].pin) == LOW); // LOW means pressed
    if (is_down != _buttons[i].previous_state) {
      // Detect state change
      if (is_down) {
        // Button just got pressed
        _buttons[i].is_pressed = true;
        _buttons[i].is_handled = false; // Ready to handle press
      } else {
        // Button just got released
        _buttons[i].is_pressed = false;
         _buttons[i].is_handled = true;
      }
    }
    // Update previous state
    _buttons[i].previous_state = is_down;
  }
}

bool ButtonController::IsButtonPressed(int btn_id){
  if(_buttons[btn_id - 1].is_pressed && !_buttons[btn_id - 1].is_handled){
      _buttons[btn_id - 1].is_handled = true;
      return true;
    } else {
      return false;
    }

}

void ButtonController::Interrupt(int _buttonId)
{
  Serial.print("Interrupt: ");
  Serial.println(_buttonId);
  if(_buttonId == 1 || _buttonId == 2 || _buttonId == 3){
    ButtonSystemEvent _event;
    _event.button_id = _buttonId;
    _event.timestamp = 500 * _buttonId;
    _data->writeEventData(_event);
  }
  else {
    ButtonSystemEvent _even = _data->popEventData();
    Serial.print("Event -----  Button: ");
    Serial.print(_even.button_id);
    Serial.print(", timestamp: ");
    Serial.println(_even.timestamp);
  }

 
}

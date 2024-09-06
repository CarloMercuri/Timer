#include "DataStorage.h"
#include <Arduino.h>

#define BTN1_PIN 8
#define BTN2_PIN 9
#define BTN3_PIN 12
#define BTN4_PIN 13

struct Button {
  int button_id;
  int pin;
  bool previous_state;
  bool is_pressed;
  bool is_handled;
};

class ButtonController
{

  public:
    /**
    * Constructor
    * Initializes the engine controller with the specified motor type, and pins
    *
    * @param yB_pin: the Arduino pin for button1.
    * @param wB_pin: the Arduino pin for button2.
    * @param rB_pin: the Arduino pin for button3.
    * @param bB_pin: the Arduino pin for button4.
    */
    ButtonController(DataStorage* data_storage, int yB_pin, int wB_pin, int rB_pin, int bB_pin);
    bool IsButtonPressed(int btn_id);
    void CheckInputs();
    void Interrupt(int pin);

  private:
  int pin_yB;
  int pin_wB;
  int pin_rB;
  int pin_bB;
  unsigned long long unixTimestamp;
  int buttonId;
  DataStorage* _data;
  Button _buttons[4];
};
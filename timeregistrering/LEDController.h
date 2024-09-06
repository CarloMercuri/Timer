class LEDController {
public:
  /**
    * Constructor
    * Initializes the engine controller with the specified motor type, and pins
    *
    * @param r_pin: the Arduino pin for stepper pin1.
    * @param g_pin: the Arduino pin for stepper pin2.
    * @param b_pin: the Arduino pin for stepper pin3.
    */
  LEDController(int r_pin, int g_pin, int b_pin);
  void SwitchOn();
  void SwitchOff();
  void SetColor(int red, int green, int blue);
  void YellowBlink();
  void RedBlink();

private:
  int pin_r;
  int pin_g;
  int pin_b;
  int r;
  int g;
  int b;
  bool is_on;
};
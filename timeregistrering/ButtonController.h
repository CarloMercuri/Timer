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
    ButtonController(int yB_pin, int wB_pin, int rB_pin, int bB_pin);
    void Interrupt(int pin);

  private:
  int pin_yB;
  int pin_wB;
  int pin_rB;
  int pin_bB;
  int unixTimestamp;
  int buttonId;
};
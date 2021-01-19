#ifndef __READBUTTON_HPP__
#define __READBUTTON_HPP__

#include <Arduino.h>

// define button pin

#define BUTTON_BREAK_TIME 700
#define BUTTON_ENABLE_TIME 3000
#define BUTTON_WAIT_TIME 600
#define ENABLE_BUTTON 1
#define DISABLE_BUTTON 0
#define ACTIVE_BUTTON 1
#define DEACTIVE_BUTTON 0

class ReadButtonClass{
  public:
  ReadButtonClass();
  ~ReadButtonClass();

  struct button_t
    {
      int count;
      int result;
    };
    

  void setup();

  button_t readbutton(int led_blink_time, bool signal_button_mode);

  private:
    int _enable_duration;
    int _result_button;
    int _count_button;
    int _wait_time;

    int _button_pin;

    bool _signal_start; 

    long _last_count_time;
    long _start_count_time;
    button_t _button;
    
};

#endif __READBUTTON_HPP__

#include "readButtonClass.hpp"

ReadButtonClass::ReadButtonClass(){
  // button will be able to be read for 3000 ms
  _enable_duration = BUTTON_ENABLE_TIME; 
  _result_button = 0;
  _count_button = 0;
  _signal_start = 0;
  _last_count_time = 0;
  _start_count_time = 0;
  _wait_time = 600;

  _button_pin = 15;
  _push_time = 0;
}

ReadButtonClass::~ReadButtonClass(){
  
}

void ReadButtonClass::setup()
{
  pinMode(_button_pin, INPUT);    // sets the digital pin  as input
}

// non block function
ReadButtonClass::button_t ReadButtonClass::readbutton(int led_blink_time, bool signal_button_mode)
{
  if(signal_button_mode == false)
  {
    _result_button = 0;
    _count_button = 0;
    _button.count = _count_button;
    _button.result = _result_button;
    return _button;
  }

  // button enable mode
  else
  {
    int button_value_current = digitalRead(_button_pin);
    long now = millis();
    
    if(_signal_start == false && button_value_current == 1){
      _count_button = 1;
      _signal_start = true;
      _last_count_time = now;
      _start_count_time = now;
      Serial.println("start");
      Serial.print("count button time: ");
      Serial.println(_count_button);
      _button.count = _count_button;
      _button.result = _result_button;
      return _button;
    }

    if(_signal_start == false && button_value_current == 0){
      _result_button = 0;
      _count_button = 0;
      _button.count = _count_button;
      _button.result = _result_button;
      return _button;
    }

    if(_signal_start == true){
      if((now - _start_count_time) <= _enable_duration){
        if((now - _last_count_time) >= _wait_time){
          if(button_value_current == 1){
            _count_button ++;
            _last_count_time = now;
            Serial.print("count button time: ");
            Serial.println(_count_button);
            _button.count = _count_button;
            _button.result = _result_button;
            return _button;
          }
          else{
            _button.count = _count_button;
            _button.result = _result_button;
            return _button;
          }

        }
        else
        {
          _button.count = _count_button;
          _button.result = _result_button;
          return _button;
        }
      }

      if((now - _start_count_time) >= _enable_duration && (now - _start_count_time) <= (_enable_duration + led_blink_time)){
          _button.count = _count_button;
          _button.result = _result_button;
          return _button;
      }
      
      if((now - _start_count_time) >= (_enable_duration + led_blink_time)){
        _result_button = _count_button;
        _count_button = 0;
        _signal_start = false;
        Serial.print("result of button: ");
        Serial.println(_result_button);
        _button.count = _count_button;
        _button.result = _result_button;
        return _button;
      } 
    }
    
  }
}

// non block function
int ReadButtonClass::long_button(bool &long_button_mode)
{
  int button_value_current = digitalRead(_button_pin);
  long now = millis();
  _push_time = 0;  
  if(_signal_start == false){ 
    delay(100);
    button_value_current = digitalRead(_button_pin);
    if(button_value_current == 1){
      _signal_start = true;
      long_button_mode = true;
      _start_count_time = now;
      Serial.println("start long button");
      _push_time = 1;
      return _push_time;
    }
    else{
      _push_time = 0;
      long_button_mode = false;
      return _push_time; 
    }
  }
  else{
    if(button_value_current == 1){
      if((now - _start_count_time)%50 == 0){
      _push_time = 1;
      return _push_time;
      }
    }
    else{
      _signal_start = false;
      long_button_mode = false;
      Serial.print("stop long button: ");
      Serial.println(long_button_mode);
      _push_time = 0;
      return _push_time;
    }
  }
  return _push_time;
}

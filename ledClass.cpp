#include "ledClass.hpp"
#include "config.hpp"
LedClass::LedClass(){
  _weather_condition_blink_time = 0;
  _hue = 42;
  _speed_led = 5;
  _heavy = NUM_LEDS;
  
  _button_enable = false;
  _last_button_count = 0;
  _last_button_blink = 0;

  _button_color.h = 192;
  _button_color.s = 255;
  _button_color.v = 255;

  _show_temperature_enable = false;
  _show_humidity_enable = false;
  _show_condition_enable = false;
  _last_show_weather = 0;
  _saturation = 300;
}

void LedClass::setup_led()
{
  FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(12)>(_leds, NUM_LEDS);
  LEDS.setBrightness(100);
  
}

void LedClass::testLed() {
  fill_solid( &(_leds[0]), NUM_LEDS /*number of leds*/, CRGB( 255, 0, 0) );
  FastLED.show();
}

void LedClass::led_button(long now, int button_count, int button_result, int led_blink_time, colorHSV_t button_color_fade)
{
  if(button_result == 0){
    if(button_count == 1 && _button_enable == false){
      _last_button_count = button_count;
      _button_enable = true;
      _last_button_blink = now;
    }
  }

  if(_button_enable){
    if((now - _last_button_blink)<= led_blink_time){
      led_change_color(_button_color.h, _button_color.s, _button_color.v);
    }
    else{
      led_change_color(button_color_fade.h, button_color_fade.s, button_color_fade.v);
      if((button_count - _last_button_count) == 1){
        _last_button_count = button_count;
        _last_button_blink = now;
      }
    }
    if(button_result != 0){
      _button_enable = false;
      _last_button_count = 0;
    }
        
  }
}



void LedClass::led_change_color(int h, int s, int v)
{
  fill_solid( &(_leds[0]), NUM_LEDS /*number of leds*/, CHSV(h, s, v) );
  FastLED.show();
}

void LedClass::led_temperature(long now, int duration, float temperature){
  if(_show_temperature_enable == false){
    _last_show_weather = now;
    _show_temperature_enable = true;
    Serial.println("temperature led on");
  }
  else{
    if((now - _last_show_weather) < duration){
      int h_t = (int)(136.8 -3.42*(temperature));
      //int h_t = (int)(160 - (16.0/9.0)*(temperature-14.0));
      fill_solid( &(_leds[0]), NUM_LEDS /*number of leds*/, CHSV(h_t, 255, 255) );
      FastLED.show();
    }
    else{
      _show_temperature_enable = false;
    }
  }
}

void LedClass::led_humidity(long now, int duration, int humidity){
  if(_show_humidity_enable == false){
    _last_show_weather = now;
    _show_humidity_enable = true;
    fade_all();
    Serial.println("humidity led on");
  }
  else{
    if((now - _last_show_weather) < duration){
      int h_h = (int) ((180.0/100.0)*(float)(humidity));
      fill_solid( &(_leds[0]), NUM_LEDS /*number of leds*/, CHSV(h_h, 255, 255) );
      FastLED.show();
    }
    else{
      _show_humidity_enable = false;
    }
  }
}

void LedClass::led_condition(long now, int duration, int weather_id){
  if(_show_condition_enable == false){
    _last_show_weather = now;
    _show_condition_enable = true;
    _saturation = 300;
    fade_all();
    Serial.println("condition led on");
    if (weather_id == 800){
      _hue = 60;
      _heavy = NUM_LEDS;
      _speed_led =5;
    }
      
    //801-804 cloud
    if (weather_id > 800 && weather_id < 900){
      _hue = 170;            
      _heavy = NUM_LEDS;
      _speed_led = 200;
      Serial.println("cloud");
    }
        
    // 700 - 799 Atmosphere
    if (weather_id >= 700 && weather_id < 800){
      _hue = 120;
      _heavy = NUM_LEDS;
      _speed_led = 30;
    }

    // 600 - 699 snow
    if (weather_id >= 600 && weather_id < 700){
      _hue = 0;
      _saturation = 0;
      _heavy = 15;
      _speed_led = 15;
    }  
        
    // 500 - 599 rain
    if (weather_id >= 500 && weather_id < 600){
      _hue = 160;
      _heavy = 15 ;
      _speed_led = 1;
    }   
          
    // 300 - 399 drizzle
    if (weather_id >= 300 && weather_id < 400){
      _hue = 224;
      _heavy = 5;
      _speed_led = 8;
    }  
    
    // 200 - 299 thunderstorm
    if(weather_id >=200 && weather_id < 300){
      _hue = 180;
      _heavy = 15;
      _speed_led = 15;
    }

    for(int i = 0; i < _heavy + 1; i++){
      if(_saturation != 300){
        _leds[i] = CHSV(_hue, _saturation, 255-i);
      }
      else{
        _leds[i] = CHSV(_hue, 255-i, 255-i);
      }
        // Serial.println("setcolor");
        //Serial.println(_hue);
    }
    if(_heavy < NUM_LEDS){
        for(int i = _heavy+1; i < NUM_LEDS ; i++) {
          _leds[i] = CRGB::Black;
        }
    } 
  }
  else{
    if((now - _last_show_weather) < duration){
      if ((now - _weather_condition_blink_time) >= _speed_led){
        memmove( &_leds[1], &_leds[0], (NUM_LEDS - 1) * sizeof( CHSV) );
        memmove( &_leds[0], &_leds[NUM_LEDS-1], 1 * sizeof( CHSV) );
        //Serial.println("show color");
        FastLED.show(); 
        _weather_condition_blink_time = now;
      }
    }
    else{
      _show_condition_enable = false;
    }
  }
}

void LedClass::fade_all()
{
  fill_solid( &(_leds[0]), NUM_LEDS /*number of leds*/, CRGB( 0, 0, 0) );
  FastLED.show();
}

#ifndef __LED_H__
#define __LED_H__

#include <FastLED.h>

#define NUM_LEDS 39
#define DATA_PIN 23
#define CLOCK_PIN 18
#define COLOR_ORDER BGR

class LedClass{
  public:
    LedClass();
    ~LedClass(){ 
    };
    struct colorHSV_t
    {
      int h;
      int s;
      int v;
    };
    colorHSV_t color_hsv;

    int tag_led;
    bool mqtt_number;
    
    bool show_temperature;
    bool show_humidity;
    bool show_weather_condition;

    void testLed(void);
    void setup_led(void);

    
    void led_button(long now, int button_count, int button_result, int led_blink_time, colorHSV_t button_color_fade);
    
    void led_temperature(long now, int duration, float tempeature);
    void led_humidity(long now, int duration, int humidity);
    void led_condition(long now, int duration, int weather_id);
    
    void led_change_color(int h, int s, int v);
    
    void fade_all();

  private:
    colorHSV_t _button_color;
    long _weather_condition_blink_time;
    int _hue;
    int _speed_led;
    int _heavy;
    int _fade_state;

    int _saturation;

    int _last_button_count;
    bool _button_enable;
    long _last_button_blink;

    bool _show_temperature_enable;
    bool _show_humidity_enable;
    bool _show_condition_enable;
    long _last_show_weather;

    CRGB _leds[NUM_LEDS];
        
};

#endif __LED_H__

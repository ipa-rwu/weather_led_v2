#ifndef __CONFIG_HPP__
#define __CONFIG_HPP__

#define LED_BLINK_TIME 200
#define SHOW_TEMPERATURE_DURATION 3600
#define SHOW_HUMIDITY_DURATION 3600
#define SHOW_CONDITION_DURATION 6000


//led state
typedef enum {
    LED_OFF     = 0,
    LED_ON    = 1,
    SHOW_BUTTON = 3,
    SHOW_TEMPERATURE_NOW = 4,
    SHOW_HUMIDITY_NOW = 5,
    SHOW_CONDITION_NOW = 6,
    SHOW_TEMPERATURE_FORECAST_MORN = 7,
    SHOW_TEMPERATURE_FORECAST_EVE = 8,
    SHOW_CONDITION_FORECAST = 9,
    CHANGE_COLOR = 2
} led_status_t;



#endif __CONFIG_HPP__


#include <WiFi.h>
#include <WiFiClient.h>
#include <string>
#include "readButtonClass.hpp"
#include "ledClass.hpp"
#include "readWeatherClass.hpp"
#include "mqttClass.hpp"
#include "config.hpp"
#include "secret.hpp"
//button
ReadButtonClass *button_obj = new ReadButtonClass();
bool button_mode;
ReadButtonClass::button_t read_button;
int pre_button_count;
long last_blink_time;
bool push_long_button_enable;

// led
led_status_t led_status;
led_status_t pre_led_status;
LedClass::colorHSV_t color_black;
int led_state = LED_OFF;
int led_blink_time = LED_BLINK_TIME;
int color_change_h;

//weather class
ReadWeatherClass *weather_obj = new ReadWeatherClass();
ReadWeatherClass::weatherData_t weather_data;
ReadWeatherClass::forecastData_t forecast_data;
String location = "stuttgart,de";
String lat = "48.7960";
String lon = "9.0096";
long last_show_weather_now;
long last_show_weather;
long last_show_weather_forecast;
long last_show_weather_forecast_now;
int show_weather_duration;
int show_forecast_duration;

//wifi create scret.hpp put this two lines
//const char* _ssid = "";
//const char* _password =
int try_wifi;

// MQTT
WiFiClient espClient;
PubSubClient client(espClient);
const char* mqtt_server_ip = "192.168.0.52";
const int   mqtt_server_port = 1883;
const char* mqtt_client_name = "weather_client";

long lastReconnectAttempt_MQTT = 0;
long lastReconnectAttempt_Wifi = 0;
MQTTClass *mqtt_obj = new MQTTClass(espClient);
const char* topic_weather = "lamp/weather"; 
const char* topic_clock = "/clock/onoff";
const char* topic_cloud = "cloud/onoff";
const char* topic_color = "lamp/color";
const char* topic_color_colock = "/clock/onoff";

//led
LedClass *led_obj = new LedClass();
char weather_msg[50];
    
void setup() {
  Serial.begin(115200);
  button_obj->setup();
  button_mode = true;
  read_button.count = 0;
  read_button.result = 0;
  pre_button_count = 0;
  push_long_button_enable = false;
  
  int try_wifi = 0;
  WiFi.mode(WIFI_STA);
  // connect to wifi
  WiFi.begin(_ssid, _password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED && try_wifi <=3) {
    try_wifi++;
    delay(1000);
    Serial.println("Connecting to WiFi..");
  } 
  if (WiFi.status() != WL_CONNECTED){
    Serial.println("Not Connected to the WiFi network");
  }
  else{
    Serial.println("Connected to the WiFi network");

  }
  
  mqtt_obj->setup(mqtt_server_ip, mqtt_server_port, mqtt_client_name);

  led_status = LED_OFF;
  led_obj->mqtt_number = false;
  led_obj->show_weather_condition = false;
  // set default light color
  led_obj->color_hsv.h = 39;
  led_obj->color_hsv.s = 182;
  led_obj->color_hsv.v = 255;
  color_black.h = 0;
  color_black.s = 0;
  color_black.v = 0;
  led_obj->setup_led();
  led_state = LED_OFF;
  pre_button_count = 0;
  pre_led_status = led_status;
  led_obj->fade_all();
  color_change_h = led_obj->color_hsv.h;

  //init weather
  weather_data.temp = 0;
  weather_data.hum = 0;
  weather_data.weather_id = 0;
  last_show_weather_now = 0;
  last_show_weather = 0;
  last_show_weather_forecast = 0;
  last_show_weather_forecast_now = 0;
  show_weather_duration = SHOW_TEMPERATURE_DURATION + SHOW_HUMIDITY_DURATION + SHOW_CONDITION_DURATION;
  show_forecast_duration = 2*SHOW_TEMPERATURE_DURATION + SHOW_CONDITION_DURATION;
}



void loop() {
  //int read_button = 0;
  long now = millis();

  //Serial.print("read button:");
  //Serial.println(read_button);
  
  // reconnect to wifi
  if ((WiFi.status() != WL_CONNECTED))
  {
    if (now - lastReconnectAttempt_Wifi > 5000)
    {
      lastReconnectAttempt_Wifi = now;
      WiFi.begin(_ssid, _password);
      if ((WiFi.status() == WL_CONNECTED))
      {
        lastReconnectAttempt_Wifi = 0;
      }
    }
  }

  // reconnect to mqtt
  if ((WiFi.status() == WL_CONNECTED))
  {
    if (!mqtt_obj->connected()) {
      if (now - lastReconnectAttempt_MQTT > 5000) {
        lastReconnectAttempt_MQTT = now;
        // Attempt to reconnect
        Serial.println("reconnet to mqtt");
        if (mqtt_obj->reconnect()) {
          lastReconnectAttempt_MQTT = 0;
        }
      }
    }
  }

  if(button_mode = true){
    read_button = button_obj->readbutton(led_blink_time, button_mode);
    if(read_button.count == 1 && read_button.result == 0){
      if(led_status != SHOW_BUTTON)
      {
        pre_led_status = led_status;
        led_status = SHOW_BUTTON;
      }
    }
    if(pre_led_status == LED_ON){
      led_obj->led_button(now, read_button.count, read_button.result, led_blink_time, led_obj->color_hsv);
    }
    if(pre_led_status == LED_OFF){
      led_obj->led_button(now, read_button.count, read_button.result, led_blink_time, color_black);
    }

    if(led_status == SHOW_BUTTON){
      if(read_button.count == 0){
        pre_button_count = 0;
        led_status = pre_led_status;
      }
    }
  }

  if(led_status == LED_OFF && read_button.result == 1)
  {
    Serial.println("turn on");
    led_obj->led_change_color(led_obj->color_hsv.h, led_obj->color_hsv.s, led_obj->color_hsv.v);
    char weather_msg[50];
    snprintf(weather_msg, 50, "onoff: %d", 1);  
    mqtt_obj->publish(topic_cloud, weather_msg); //Topic name 
    led_status = LED_ON;
    return;
  }

  if(led_status == LED_OFF && read_button.result == 2)
  {
    char weather_msg[50];
    snprintf(weather_msg, 50, "onoff: %d", 1);  
    mqtt_obj->publish(topic_clock, weather_msg); //Topic name 
    Serial.println("turn on clock");
    return;
  }

  if(led_status == LED_OFF && read_button.result == 3)
  {
    char weather_msg[50];
    snprintf(weather_msg, 50, "onoff: %d", 0);  
    mqtt_obj->publish(topic_clock, weather_msg); //Topic name 
    Serial.println("turn off clock");
    return;
  }
  
  if(led_status == LED_ON && read_button.result == 4)
  {
    Serial.println("turn off");
    char weather_msg[50];
    snprintf(weather_msg, 50, "onoff: %d", 0);  
    mqtt_obj->publish(topic_cloud, weather_msg); //Topic name 
    led_obj->fade_all();
    led_status = LED_OFF;
  }
//  if(led_status == LED_ON && read_button.result >= 5)

  if(read_button.result >= 5)
  {
    if(led_status != CHANGE_COLOR)
    {
      pre_led_status = led_status;
      led_status = CHANGE_COLOR;
    }
    Serial.println("will change color");
    push_long_button_enable = true;
    button_mode = false;
  }

  if(led_status == CHANGE_COLOR){
    int push_button = 0;
    button_mode == false;
    push_button = button_obj->long_button(push_long_button_enable);
    if(push_long_button_enable == true && push_button == 1){
      Serial.println("change color");
      color_change_h ++;
      if(color_change_h == 255){
        color_change_h = 0;
      }
      led_obj->led_change_color(color_change_h, led_obj->color_hsv.s, led_obj->color_hsv.v);
      char color_msg[50];
      snprintf(color_msg, 50, "color: %d,%d,%d", color_change_h, led_obj->color_hsv.s, led_obj->color_hsv.v);  
      mqtt_obj->publish(topic_color_colock, color_msg); //Topic name 
      button_mode == false;
    }
    if(push_long_button_enable == false){
      Serial.println("stop color");
      button_mode = true;
      led_status = pre_led_status;
      if(led_status == LED_OFF){
        led_obj->fade_all();
      }
      else{
        led_obj->color_hsv.h = color_change_h;
      }
    }
  }

  if(led_status == LED_ON && read_button.result == 1){
    led_status = SHOW_TEMPERATURE_NOW;
    button_mode = false;
    weather_data = weather_obj->readweather(location);
    Serial.println("will show weather now");
    char weather_msg[50];
    // snprintf(weather_msg, 50, "{\"temperature\": %1f}", ((weather_data.temp-32)/1.8));  
    snprintf(weather_msg, 50, "temperature: %1f", (weather_data.temp));  
    Serial.print("Publish message: ");
    Serial.println(weather_msg);      
    mqtt_obj->publish(topic_weather, weather_msg); //Topic name 
    last_show_weather_now = now;
    last_show_weather = now;
  }

  if(led_status != LED_OFF && \
    led_status != SHOW_BUTTON && \
    led_status != SHOW_TEMPERATURE_FORECAST_MORN && \
    led_status != SHOW_TEMPERATURE_FORECAST_EVE && \
    led_status != SHOW_CONDITION_FORECAST){
      
    if(led_status == SHOW_TEMPERATURE_NOW || \
      led_status == SHOW_HUMIDITY_NOW|| \
      led_status == SHOW_CONDITION_NOW){
      if((now - last_show_weather_now) < show_weather_duration){
        if(led_status == SHOW_TEMPERATURE_NOW){
          if((now - last_show_weather) < SHOW_TEMPERATURE_DURATION){
            led_obj->led_temperature(now , SHOW_TEMPERATURE_DURATION, weather_data.temp);
          }
          else{
            led_status = SHOW_HUMIDITY_NOW;
            last_show_weather = now;
          }
        }
        if(led_status == SHOW_HUMIDITY_NOW){
          if((now - last_show_weather) < SHOW_HUMIDITY_DURATION){
            led_obj->led_humidity(now , SHOW_HUMIDITY_DURATION, weather_data.hum);
          }
          else{
            led_status = SHOW_CONDITION_NOW;
            last_show_weather = now;
          }
        } 
        if(led_status == SHOW_CONDITION_NOW){
          if((now - last_show_weather) < SHOW_CONDITION_DURATION){
            led_obj->led_condition(now , SHOW_CONDITION_DURATION, weather_data.weather_id);
          }
        }
      }
      else{
        led_obj->led_change_color(led_obj->color_hsv.h, led_obj->color_hsv.s, led_obj->color_hsv.v);
        Serial.println("back normal");
        led_status = LED_ON;
        button_mode = true;
      }
    }
  }

  if(led_status == LED_ON){
    // today
    if(read_button.result == 2){
      forecast_data = weather_obj->forecast(lat, lon, 0);
      Serial.println("will forecast weather today");
      led_status = SHOW_TEMPERATURE_FORECAST_MORN;
      button_mode = false;
    }
    
    // tomorrow
    if(read_button.result == 3){
      forecast_data = weather_obj->forecast(lat, lon, 1);
      Serial.println("will forecast weather tomorrow");
      led_status = SHOW_TEMPERATURE_FORECAST_MORN;
      button_mode = false;
    }

    if(led_status == SHOW_TEMPERATURE_FORECAST_MORN){
      char morn_msg[50];
      snprintf(morn_msg, 50, "temperature_morn: %1f", (forecast_data.temp_morn));  
      Serial.print("Publish message: ");
      Serial.println(morn_msg);      
      mqtt_obj->publish(topic_weather, morn_msg); //Topic name 
  
      delay(100);
      
      char eve_msg[50];
      snprintf(eve_msg, 50, "temperature_eve: %1f", (forecast_data.temp_eve));  
      Serial.print("Publish message: ");
      Serial.println(eve_msg);      
      mqtt_obj->publish(topic_weather, eve_msg); //Topic name 
      
      last_show_weather_forecast = now;
      last_show_weather_forecast_now = now; 
    }
  }
  
  if(led_status != LED_OFF && \
    led_status != SHOW_BUTTON && \
    led_status != SHOW_TEMPERATURE_NOW && \
    led_status != SHOW_HUMIDITY_NOW && \
    led_status != SHOW_CONDITION_NOW){
    if(led_status == SHOW_TEMPERATURE_FORECAST_MORN || \
      led_status == SHOW_TEMPERATURE_FORECAST_EVE|| \
      led_status == SHOW_CONDITION_FORECAST){
        
      if((now - last_show_weather_forecast) < show_forecast_duration){
        if(led_status == SHOW_TEMPERATURE_FORECAST_MORN){
          if((now - last_show_weather_forecast_now) < SHOW_TEMPERATURE_DURATION){
            led_obj->led_temperature(now , SHOW_TEMPERATURE_DURATION, forecast_data.temp_morn);
          }
          else{
            led_status = SHOW_TEMPERATURE_FORECAST_EVE;
            last_show_weather_forecast_now = now;
          }
        }
        if(led_status == SHOW_TEMPERATURE_FORECAST_EVE){
          if((now - last_show_weather_forecast_now) < SHOW_TEMPERATURE_DURATION){
            led_obj->led_temperature(now , SHOW_TEMPERATURE_DURATION, forecast_data.temp_eve);
          }
          else{
            led_status = SHOW_CONDITION_FORECAST;
            last_show_weather_forecast_now = now;
          }
        } 
        if(led_status == SHOW_CONDITION_FORECAST){
          if((now - last_show_weather_forecast_now) < SHOW_CONDITION_DURATION){
            led_obj->led_condition(now , SHOW_CONDITION_DURATION, forecast_data.weather_id);
          }
        }
      }
      else{
        led_obj->led_change_color(led_obj->color_hsv.h, led_obj->color_hsv.s, led_obj->color_hsv.v);
        Serial.println("back normal");
        led_status = LED_ON;
        button_mode = true;
      }
    }
  }
  mqtt_obj->loop();
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

  /*
  if(button_mode = true){
    read_button = button_obj->readbutton(led_blink_time, button_mode);
    if(pre_button_count != read_button.count && read_button.result == 0)
    {
      if(led_status != SHOW_BUTTON)
      {
        pre_led_status = led_status;
        last_blink_time = now;
        led_status = SHOW_BUTTON;
      }
      if ((now - last_blink_time) < led_blink_time){
        led_obj->led_change_color(192, 255, 255);
      }
      else{
        if(pre_led_status == LED_ON){
          led_obj->led_change_color(led_obj->color_hsv.h, led_obj->color_hsv.s, led_obj->color_hsv.v);
        }
        if(pre_led_status == LED_OFF){
          led_obj->fade_all();
        }
        
        pre_button_count = read_button.count;
      } 
    }
    // finsih counting
    if(led_status == SHOW_BUTTON){
      if(pre_button_count == read_button.count){
        last_blink_time = now;  
      }

      //still counting
      if(read_button.count == 0){
        pre_button_count = 0;
        led_status = pre_led_status;
      }
    }
  }
  */

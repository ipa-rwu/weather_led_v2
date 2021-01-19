#ifndef READWEATHER_HPP
#define READWEATHER_HPP

#include <HTTPClient.h>
#include <ArduinoJson.h>

class ReadWeatherClass{
  public:
    ReadWeatherClass();
    ~ReadWeatherClass();
    
    struct weatherData_t
    {
      float temp;
      int hum;
      int weather_id;
    };

    struct forecastData_t {
      float temp_morn;
      float temp_eve;
      int weather_id;
    };
    
    weatherData_t readweather(String location);

    forecastData_t forecast(String lat, String lon, int date);

   private:
    String _unit;
    String _key;

    forecastData_t _forecast_data;
    weatherData_t _weather_data;

};

#endif 

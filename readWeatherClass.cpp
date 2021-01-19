#include "readWeatherClass.hpp"

ReadWeatherClass::ReadWeatherClass()
{
  _unit = "&units=metric";
  _key = "";
}

ReadWeatherClass::~ReadWeatherClass()
{
}


ReadWeatherClass::weatherData_t ReadWeatherClass::readweather(String location){
  
  HTTPClient http;
  String payload;
  String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=" + location + _unit + "&appid=";
  http.begin(endpoint + _key);
 //   http.begin(url, fingerprint);
  int httpCode = http.GET();

  if (httpCode > 0) { //Check for the returning code
 
    payload = http.getString();
    //Serial.println(httpCode);
    Serial.println(payload);

    StaticJsonDocument<1024> doc;

     // Parse JSON object
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
       Serial.print(F("deserializeJson() failed: "));
       Serial.println(error.c_str());
       _weather_data.temp = 0.0;
       _weather_data.hum = 0;
       _weather_data.weather_id = 0;
       return _weather_data;
     }
    else {
      _weather_data.temp = doc["main"]["feels_like"];
      _weather_data.hum = doc["main"]["humidity"];
      _weather_data.weather_id = doc["weather"][0]["id"];
      //Serial.print("Hum = "); Serial.println(_weather_data.hum);
      Serial.println("weather_id = "); 
      Serial.print(_weather_data.weather_id);
      }
   }
 
    else {
      Serial.println("Error on HTTP request");
    }

    http.end();
    return _weather_data;
}

// date: 0 today 1 tommorrow
ReadWeatherClass::forecastData_t ReadWeatherClass::forecast(String lat, String lon, int date){
  
  HTTPClient http;
  String payload; 
  String endpoint = "http://api.openweathermap.org/data/2.5/onecall?lat=" + lat + "&lon=" + lon + "&units=metric" + "&exclude=hourly,current&appid=";
  

  http.begin(endpoint + _key);
 //   http.begin(url, fingerprint);
  int httpCode = http.GET();
  
  if (httpCode > 0) 
  { //Check for the returning code
 
    payload = http.getString();
    //Serial.println(httpCode);
    //Serial.println(payload);

    StaticJsonDocument<2048> doc;

     // Parse JSON object

    StaticJsonDocument<200> filter;
    filter["daily"][0]["feels_like"] = true;
    filter["daily"][0]["weather"][0]["id"] = true;
  
    DeserializationError error = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
    //DeserializationError error = deserializeJson(doc, payload);
    if (error){
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      _forecast_data.temp_morn = 0.0;
      _forecast_data.temp_morn = 0.0;
      _forecast_data.weather_id = 0;
      return _forecast_data;
    }
   else {
        //serializeJsonPretty(doc, Serial);

      //current_timestamp = doc["daily"]["dt"]
      _forecast_data.temp_morn = doc["daily"][date]["feels_like"]["morn"];
      _forecast_data.temp_eve = doc["daily"][date]["feels_like"]["eve"];
      _forecast_data.weather_id = doc["daily"][date]["weather"][0]["id"];
      Serial.print("weather_id = "); 
      Serial.println(_forecast_data.weather_id);
      }
   }
 
  else 
  {
    Serial.println("Error on HTTP request");
  }

  http.end();
  return _forecast_data;
}

#ifndef __MQTT_HPP__
#define __MQTT_HPP__

#include <WiFiClient.h>
#include <PubSubClient.h>
#include <string>

#define MAX_SIZE_MY_ID 16

class MQTTClass: public PubSubClient
{
  public:
    MQTTClass(Client& wifiClient);
    ~MQTTClass();
    
    void setup(const char *serverIP = "192.168.1.1", int port = 1883, const char *myId = "ESP-SENSOR");
    bool reconnect();   

    const char *topic_location, *topic_ball_color, *topic_onoff;

    String str_payload;
    
  private:
    IPAddress _serverIPAddress;
    int _serverPort;
    char _id[MAX_SIZE_MY_ID];   // hostname of the current device (i.e. ESP8266)

  protected:
    virtual void onCallback(char* topic, byte* payload, unsigned int length);

};


#endif __MQTT_HPP__

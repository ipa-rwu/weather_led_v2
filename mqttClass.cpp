#include "mqttClass.hpp"

MQTTClass::MQTTClass(Client& wifiClient):PubSubClient(wifiClient)
{

  topic_location = "ball/weather/location";
  topic_ball_color = "ball/color";
  topic_onoff = "ball/onoff";
}

MQTTClass::~MQTTClass()
{
}

void MQTTClass::setup(const char* serverIP, int port, const char* myId) {
  this->_serverIPAddress.fromString(serverIP);
  this->_serverPort = port;
  setServer(_serverIPAddress, _serverPort);
  Serial.print("serverIP: ");
  Serial.print(serverIP);
  Serial.print(" port: ");
  Serial.print(port);
    
  strncpy(_id, myId, MAX_SIZE_MY_ID);
  Serial.print(" id: ");
  Serial.println(_id);
  
  if (!this->connect(_id)) {
    Serial.print("failed, rc=");
    Serial.print(this->state());
    Serial.println(" try again in 5 seconds");
  }
}

bool MQTTClass::reconnect() {
  if (this->connect(_id)) {
    // Once connected, publish an announcement...
    Serial.print("connected to mqtt");
    Serial.println();
    publish("esp/reconnect", "hello"); //Topic names
  }
  else
  {
    Serial.print("Not connect to mqtt, try again");
    Serial.println();
  }
  return connected();
}

void MQTTClass::onCallback(char* topic, byte* payload, unsigned int length) 
{

  if (strcmp(topic, topic_location) == 0 || \ 
  strcmp(topic, topic_ball_color) == 0 || \
  strcmp(topic, topic_onoff) == 0)
  {
    payload[length] = '\0';
    //for (int i = 0; i < length; i++) {
    //  Serial.println((char)payload[i]);
    //}
    // str_payload is payload of msg
    str_payload = (char *)payload; 
  }
 
}

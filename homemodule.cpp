/*
 * HomeModule - library for connecting to MQTT and publishing
 * registration and status messages.
 */
#include "Arduino.h"
#include "homemodule.h"
#include "homemoduleconfig.h"
#include <PubSubClient.h>

// Constructor taking name of the MQTT server
HomeModule::HomeModule(const char* mqttServer, WiFiClient espClient)
{
  localSetup(mqttServer, espClient);
}

// Constructor will callback for subscription
HomeModule::HomeModule(const char* mqttServer, MQTT_CALLBACK_SIGNATURE, WiFiClient espClient)
{
  localSetup(mqttServer, espClient);
  _mqClient->setCallback(callback);
}

void HomeModule::localSetup(const char* mqttServer, WiFiClient espClient)
{
  _mqttServer = mqttServer;
  _mqClient = new PubSubClient( espClient );
  _mqClient->setServer(_mqttServer, 1883);
  _pollTime = HM_REGISTER_POLL_MILLISECONDS;
}

boolean HomeModule::setupHomeModule(const char* deviceName, const char* deviceType, const char* deviceLocation, IPAddress deviceAddress)
{
    Serial.print("Setting up home module");
    _name = deviceName;
    _type = deviceType;
    _location = deviceLocation;
    _address = deviceAddress;
    _lastRegister = 0;
    return true;
}

boolean HomeModule::loop()
{
  if ( !_mqClient->loop() ) {
    reconnect();
  } else {
    registerDevice();
  }
  return true;
}

// Connect to MQTT. Subscribe to topic home/DEVICE_LOCATION/DEVICE_TYPE/MDNS_NAME
void HomeModule::reconnect() {
  char topic[50];
  // Loop until we're reconnected
  while (!_mqClient->connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print( _mqttServer );
    Serial.print( "..." );
    // Attempt to connect
    if (_mqClient->connect(_name)) {
      Serial.println("Connected to MQTT broker.");
      // Subscribe to topic home/LOCATION/DEVICE_TYPE/NAME
      sprintf( topic, "home/%s/%s/%s", _location, _type, _name);
      Serial.print("Subscribing to topic ");
      Serial.println(topic);
      _mqClient->subscribe( topic );
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(_mqClient->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Renew registration with controller every 60 seconds
void HomeModule::registerDevice()
{
  long now = millis();
  if ( _lastRegister == 0 || ( now - _lastRegister > _pollTime ) ) {
    _lastRegister = now;
    char message[50];
    // Publish registration details to home/devices/register
    sprintf( message, "%s,%s,%s", _name, _type, _location);
    Serial.print("Publishing to topic ");
    Serial.print(HM_MQTT_REGISTRATION_TOPIC);
    Serial.print(": ");
    Serial.println(message);
    _mqClient->publish(HM_MQTT_REGISTRATION_TOPIC, message);
  }
}

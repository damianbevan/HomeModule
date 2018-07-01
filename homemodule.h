/* Home module generic definitions * 
 */
#ifndef homemodule_h
#define homemodule_h
#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define HM_MQTT_REGISTRATION_TOPIC "home/devices/register" // The name of the registration topic
//#define HM_MQTT_SERVER "broker.hivemq.com"                 // The MQTT broker hostname
#define HM_REGISTER_POLL_MILLISECONDS 60000                // The frequency the device publishes a registration message

/* Device Constants */
#define HM_LOCATION_LIVINGROOM "LIVINGROOM"
#define HM_LOCATION_OFFICE "OFFICE"

#define HM_DEVICE_TYPE_TEST "TEST"
#define HM_DEVICE_TYPE_VW_VAN "VW_VAN"

class HomeModule
{
  public:
    // Constructor for HomeModule
    HomeModule( const char* mqttServer, WiFiClient& );

    HomeModule( const char* mqttServer, MQTT_CALLBACK_SIGNATURE, WiFiClient& );
    
    // Register the Home Module with the server
    boolean setupHomeModule(const char* deviceName, const char* deviceType, const char* deviceLocation, IPAddress deviceAddress);

    // Loop - heartbeat and callbacks.
    boolean loop();

  private:
    void localSetup(const char* mqttServer, WiFiClient& espClient );
    void reconnect();
    void registerDevice();
    const char* _mqttServer;
    const char* _name;
    const char* _type;
    const char* _location;
    IPAddress _address;
    PubSubClient *_mqClient;
    unsigned int _pollTime;
    unsigned long _lastRegister;
};

#endif

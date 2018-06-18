/*
 *  Home Module to control a vehical module
 */
#include "homemoduleconfig.h"
#include "homemodule.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266mDNS.h>
//#include <ESP8266WebServer.h>
#include <PubSubClient.h>

// Create an instance of the web server
// specify the port to listen on as an argument
//ESP8266WebServer server(80);

// Declare callback function
void mqCallback(char* topic, byte* payload, unsigned int length);

/* MQTT Constants */
HomeModule homeModule( HM_MQTT_SERVER, mqCallback );
//const char* MQTT_REGISTRATION_TOPIC = HM_MQTT_REGISTRATION_TOPIC;     // The name of the registration topic
//const char* MQTT_SERVER = HM_MQTT_SERVER;                             // The MQTT broker hostname
//const int REGISTER_POLL_MILLISECONDS = HM_REGISTER_POLL_MILLISECONDS; // The frequency the device publishes a registration message

/* Device Constants */
const char* MDNS_NAME="esp8266_test1";                  // The name of this device
const char* DEVICE_TYPE = HM_DEVICE_TYPE_TEST;        // The device type
const char* DEVICE_LOCATION = HM_LOCATION_LIVINGROOM; // The location of this device
const char* DEVICE_KEY = HM_DEVICE_1_JWT_KEY;         // Device JWT authn/authz key

/* Local Variables */
ESP8266WiFiMulti wifiMulti;
WiFiClient espClient;
//PubSubClient mqClient(espClient);
//long lastRegister = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  setupWifi();
  setupmDNS();
  setupHomeModule();
//  setupMQTT();
  setupGPIO();
  // Start the HTTP server
/*  server.on("/", HTTP_GET, handleRoot);
  server.on("/LED", HTTP_POST, handleLED);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP Server started");*/
}

void mqCallback(char* topic, byte* payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(D7, HIGH);  // Turn the LED on by making the voltage HIGH
    Serial.println("D7=ON");
  } else if ((char)payload[0] == '0') {
    digitalWrite(D7, LOW);  // Turn the LED off by making the voltage LOW
    Serial.println("D7=OFF");
  } else if ((char)payload[0] == 'y') {
    digitalWrite(LED_BUILTIN, LOW);     // Turn the LED on (Note that LOW is the voltage level
                                        // but actually the LED is on; this is because
                                        // it is acive low on the ESP-01)
    Serial.println("LED_BUILTIN=ON");
  } else if ((char)payload[0] == 'n') {
    digitalWrite(LED_BUILTIN, HIGH);     // Turn the LED off by making the voltage HIGH
    Serial.println("LED_BUILTIN=OFF");
  }
}

void setupHomeModule()
{
  homeModule.setupHomeModule( MDNS_NAME, DEVICE_TYPE, DEVICE_LOCATION, WiFi.localIP() );
}

void setupmDNS()
{
  if ( !MDNS.begin( MDNS_NAME )) {
    Serial.println("Error setting up MDNS responder");
  }
  Serial.println("mDNS responder started.");
  Serial.print( "Connnect to http://");
  Serial.print( MDNS_NAME );
  Serial.println( ".local/");
}

void setupGPIO()
{
  // prepare GPIO7
  pinMode(D7, OUTPUT);
  digitalWrite(D7, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
}

void setupWifi()
{
  // Add wifi Access Points to wifi
    //wifiMulti.addAP( HM_WIFI_SSID_PHONE, HM_WIFI_PWD_PHONE ); // Phone
    wifiMulti.addAP( HM_WIFI_SSID_HOME, HM_WIFI_PWD_HOME ); // Home
  
  // Connect to WiFi network
  Serial.print("Connecting to wifi...");

  int i = 0;
  while ( wifiMulti.run() != WL_CONNECTED) { // Wait for the wifi to connect. Scan for the strongest signal and connect.
    delay(1000);
    Serial.print( i++ );
    Serial.print(".");
  }
  
  Serial.println("\n");
  Serial.print("WiFi connected to ");
  Serial.println(WiFi.SSID());

  // Print the IP address
  Serial.print( "IP Address: " );
  Serial.println(WiFi.localIP());
}
/*
// Connect to MQTT. Subscribe to topic home/DEVICE_LOCATION/DEVICE_TYPE/MDNS_NAME
void reconnect() {
  // Loop until we're reconnected
  while (!mqClient.connected()) {
    Serial.print("Attempting MQTT connection to ");
    Serial.print( MQTT_SERVER );
    Serial.print( "..." );
    // Attempt to connect
    if (mqClient.connect(MDNS_NAME)) {
      Serial.println("Connected to MQTT broker.");
      // Subscribe to topic home/LOCATION/DEVICE_TYPE/NAME
      sprintf( topic, "home/%s/%s/%s", DEVICE_LOCATION, DEVICE_TYPE, MDNS_NAME );
      Serial.print("Subscribing to topic ");
      Serial.println(topic);
      mqClient.subscribe( topic );
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
*/

/*
// Renew registration with controller every 60 seconds
void registerDevice()
{
  long now = millis();
  if ( lastRegister == 0 || ( now - lastRegister > (REGISTER_POLL_MILLISECONDS) ) ) {
    lastRegister = now;
    char message[50];
    // Publish registration details to home/devices/register
    sprintf( message, "%s,%s,%s", MDNS_NAME, DEVICE_TYPE, DEVICE_LOCATION );
    Serial.print("Publishing to topic ");
    Serial.print(MQTT_REGISTRATION_TOPIC);
    Serial.print(": ");
    Serial.println(message);
    mqClient.publish(MQTT_REGISTRATION_TOPIC, message);
  }
}
*/
void loop()
{
  homeModule.loop();
  delay(10);
  //server.handleClient();
}

/*
void handleRoot(){
   server.send(200, "text/html", "<form action=\"/LED\" method=\"POST\"><input type=\"submit\" value=\"Toggle LED\"></form>");
}

void handleLED(){
  digitalWrite(D7, !digitalRead(D7));
  server.sendHeader("Location","/");
  server.send(303);
}

void handleNotFound(){
  server.send(404, "text/plain", "404: Not found");
}
*/
/*
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();
  
  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1)
    val = LOW;
  else if (req.indexOf("/gpio/1") != -1)
    val = HIGH;
  else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  // Set GPIO7 according to the request
  digitalWrite(D7, val);
  
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\nGPIO D7 is now ";
  s += (val)?"high":"low";
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}
*/

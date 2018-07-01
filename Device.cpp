#include "stdafx.h"
#include "Device.h"
#ifdef ESP8266
#include <ArduinoJson.h>
#endif

/* Constructor */
Device::Device(const char * name, const char * identifier)
{
	_name = name;
	_identifier = identifier;
}

/* Loop the device and perform updates */
void Device::loop()
{
	printf("Looping in Device\n");
}

/* Setup the device*/
void Device::setup()
{
	printf("Setup Device\n");
}

/* Set the controller for the LEDs*/
void Device::setLEDController(LEDController* ledController)
{
	Device::_ledController = ledController;
}

/* Get the status of the device by querying each controller */
int Device::getStatus()
{
	return 0;
}

/* Process a JSON message and call the appropriate device driver */
boolean Device::processMessage(char* json)
{
	boolean processed = false;

	// Convert JSON Message to LEDMessage structure
	if (NULL != _ledController) {
		// Extract LEDController message and pass it to the LED Controller
		LEDMessage ledMessage = LEDMessage();
#ifdef ESP8266
		// Do some Arduino JSON stuff
		const size_t bufferSize = 3 * JSON_ARRAY_SIZE(2) + JSON_ARRAY_SIZE(3) + 2 * JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + 160;
		DynamicJsonBuffer jsonBuffer(bufferSize);

		//const char* json = "{\"type\":\"device\",\"version\":1,\"apiKey\":\"1234ABCD\",\"lights\":{\"version\":1,\"on\":[1,2,3],\"off\":[4,5],\"flash\":{\"on\":[6,7],\"off\":[9,10],\"interval\":5}},\"servo\":{\"version\":3,\"id\":3,\"rotation\":90}}";

		JsonObject& root = jsonBuffer.parseObject(json);
		processed = root.success();
		if (!processed) {
			Serial.print(json);
			Serial.println("JSON Paring failed.");
			return processed;
		}
		const char* type = root["type"]; // "device"
		int version = root["version"]; // 1
		const char* apiKey = root["apiKey"]; // "1234ABCD"

		if (root.containsKey("lights")) {
			JsonObject& lights = root["lights"];
			int lights_version = lights["version"]; // 1

			JsonArray& lights_on = lights["on"];
			int numberOfElements = lights_on.size();
			for (int i = 0; i < numberOfElements; i++) {
				if (lights_on[i] < MAX_LEDS) {
					ledMessage.on[lights_on[i]] = true;
				}
			}

			JsonArray& lights_off = lights["off"];
			numberOfElements = lights_off.size();
			for (int i = 0; i < numberOfElements; i++) {
				if (lights_off[i] < MAX_LEDS) {
					ledMessage.off[lights_off[i]] = true;
				}
			}
			/* Future JSON fields
			JsonObject& lights_flash = lights["flash"];
			int lights_flash_on0 = lights_flash["on"][0]; // 6
			int lights_flash_on1 = lights_flash["on"][1]; // 7
			int lights_flash_off0 = lights_flash["off"][0]; // 9
			int lights_flash_off1 = lights_flash["off"][1]; // 10
			int lights_flash_interval = lights_flash["interval"]; // 5

			if (root.containsKey("servo")) {
				JsonObject& servo = root["servo"];
				int servo_version = servo["version"]; // 3
				int servo_id = servo["id"]; // 3
				int servo_rotation = servo["rotation"]; // 90
			}
		*/
		}
#else
		// Test message for VC
		ledMessage.on[0] = true;
		ledMessage.on[1] = true;
		ledMessage.on[2] = true;
		ledMessage.on[3] = true;
		ledMessage.on[4] = true;
		ledMessage.on[5] = true;
		ledMessage.on[6] = true;
		ledMessage.off[5] = true;
		ledMessage.off[6] = true;
		ledMessage.off[7] = true;
#endif
		processed = _ledController->processMessage(&ledMessage);
	}

	return processed;
}

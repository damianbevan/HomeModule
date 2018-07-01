#pragma once
#include "LEDController.h"

class Device
{
public:
	Device( const char* name, const char* identifier);
	int getStatus();
	void loop();
	void setup();
	void setLEDController(LEDController* ledController);
	boolean processMessage(char* message);
private:
	const char* _name;
	const char* _identifier;
	LEDController* _ledController;
};


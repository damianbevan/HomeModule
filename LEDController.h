#pragma once
#define MAX_LEDS 8
#define MAX_CURRENT_MA 50
#define LED_CURRENT_MA 10
#define LED_ON true
#define LED_OFF false

struct LEDMessage {
  int on[MAX_LEDS] = { 0 };
  int off[MAX_LEDS] = { 0 };
  int flash_on[MAX_LEDS] = { 0 };
  int flash_off[MAX_LEDS] = { 0 };
  int flash_interval = 0;
};

class LEDController
{
public:
  LEDController();
  boolean processMessage(LEDMessage*);
  boolean setAllOff();
private:
  boolean calculateMaxPowerExceeded(LEDMessage*);
  boolean setLEDOnOff(int ledNumber, boolean onoff);
  boolean setLEDflash(int ledNumber[], boolean onoff, int interval);
  int ledState[MAX_LEDS] = { 0 };
  void printStatus();
};



#include "stdafx.h"
#include "LEDController.h"

#define COUNTOF(x) (sizeof(x)/sizeof(*x))

int* intdup(int const * src, size_t len);

/* Constructor */
LEDController::LEDController()
{
}

// Process an LED controll message and set LEDs on, off or flashing.
boolean LEDController::processMessage(LEDMessage* ledMessage)
{
  printStatus();
  if (!calculateMaxPowerExceeded(ledMessage))
  {
    int onArraySize = COUNTOF(ledMessage->on);
    for (int i = 0; i < onArraySize; i++) {
      if (ledMessage->on[i]) {
        setLEDOnOff(i, true);
      }
    }
    int offArraySize = COUNTOF(ledMessage->off);
    for (int i = 0; i < offArraySize; i++) {
      if (ledMessage->off[i]) {
        setLEDOnOff(i, false);
      }
    }
    printStatus();
  }
  return boolean();
}

/* Print a visual representation of the LED statuses*/
void LEDController::printStatus()
{
  char tmp[MAX_LEDS + 1] = "";
  for (int i = 0; i < MAX_LEDS; i++) {
    if (ledState[i]) {
      tmp[i] = 'O';
    }
    else {
      tmp[i] = 'X';
    }
  }
  printf("LEDController Status: %s\n", tmp);

}

/* Calculate whether the maximum current will be exceeded.	*/
/* Return true if it will be and false otherwise.			*/
boolean LEDController::calculateMaxPowerExceeded(LEDMessage * ledMessage)
{
  boolean currentExceeded = false;

  // Create a copy of the current state
  int* ledStateCopy = intdup(ledState, (size_t)MAX_LEDS);
  if (NULL == ledStateCopy) {
	  printf("Failed to allocate memory in calculateMaxPowerExceeded");
	  // Ensure no LEDs are turned on.
	  return true;
  }

  // Turn LEDS on and off in copy
  for (int i = 0; i < MAX_LEDS; i++) {
    if (ledMessage->on[i]) {
      ledStateCopy[i] = LED_ON;
    }
	if (ledMessage->off[i]) {
		ledStateCopy[i] = LED_OFF;
	}
  }

  // Loop through and add up total LED current
  int totalCurrentMA = 0;
  for (int i = 0; i < MAX_LEDS; i++) {
    if (ledStateCopy[i]) {
		totalCurrentMA += LED_CURRENT_MA;
    }
  }

  // If current exceeded 
  if (totalCurrentMA > MAX_CURRENT_MA) {
    currentExceeded = true;
    printf("MAXIMUM CURRENT EXCEEDED ON LED DISPLAY. Current: %dmA\n", totalCurrentMA);
  }

  // Free memory and return
  free(ledStateCopy);

  return currentExceeded;
}

/* Duplicate an int array */
int* intdup(int const * src, size_t len)
{
  int * p = (int*)malloc(len * sizeof(int));
  if ((NULL != memcpy(p, src, len * sizeof(int)))) {
    return p;
  }
  return NULL;
}

/* Set an LEDs on or off*/
boolean LEDController::setLEDOnOff(int ledNumber, boolean onoff)
{
  printf("Setting LED %d %s\n", ledNumber, onoff ? "ON" : "OFF");
  ledState[ledNumber] = onoff;
  return true;
}

/* Seta ll LEDs off */
boolean LEDController::setAllOff()
{
  printf("Setting all LEDs off\n");
  return true;
}


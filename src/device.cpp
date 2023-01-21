#include <Bounce2.h>

#include "device.h"
#include <Arduino.h>
#include "WiFi.h"
#include "configuration.h"
#include "debugger.h"
#include "settings.h"

#define MINUTES_2 120000
// PINS!!! Project definitions

#define LEDPIN          14  //  Relay pin GPIO 14
#define LEDPIN_STATUS_OFF 0       // LED is allways off
#define LEDPIN_STATUS_ON 1        // LED is allways on
#define LEDPIN_STATUS_FOLLOW 2    // Follow the light (light on, led on etc)
#define LEDPIN_STATUS_INVERTED 4  // Follow the light inverted (light off, led on etc)

#define LED_OFF 0
#define LED_ON 1

#define LEDSTRIPPIN1 12 // FIXME esp port 1 GPIO 12
#define LEDSTRIPPIN2 13 // FIXME	esp port 2 GPIO 13
#define TIMERDETECTPIN 5 // FIXME esp port 3 GPIO 5

#define LIGHT_FULL_ON  255 // FIXME final needs to be 255.
#define LIGHT_FULL_OFF 0



int ledStatus = LEDPIN_STATUS_FOLLOW;  // default LED is inverted to light

Device device;
// mDNS
#define CONFIG_MDNS_NAME DEFAULT_CONFIG_DEVICENAME
static const uint8_t D11 = 9;
static const uint8_t D12 = 10;

// Instantiate a Bounce object
Bounce voltageTimerBounce = Bounce();
bool voltageTimer = false;

const uint32_t LED_DELAY = 2000; // milliseconds
uint32_t delayTime = 0; // milliseconds

static uint32_t timeLastTransition = 0;
static enum { LIGHT_ON, LIGHT_OFF, LIGHT_FADE_IN, LIGHT_FADE_OUT, WAIT_DELAY } state;
static uint8_t lastState = WAIT_DELAY;


// ____________________________________________________________________________________________________________________
//  Name:   void setupDevice()
//  Usage:  setup the device
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________
void setLED(bool status) {
    // NOTICE: LED on is status LOW
    // NOTICE: LED off is status HIGH

    switch (config.ledStatus) {
    case LEDPIN_STATUS_OFF:  // LED allways off
        digitalWrite(LEDPIN, HIGH);
        break;
    case LEDPIN_STATUS_ON:  // LED allways on
        digitalWrite(LEDPIN, LOW);
        break;
    case LEDPIN_STATUS_FOLLOW:  // LED follows the light
        if (status == LED_ON) {
            digitalWrite(LEDPIN, LOW);
        }
        else {
            digitalWrite(LEDPIN, HIGH);
        }
        //
        break;
    case LEDPIN_STATUS_INVERTED:  // LED follows the light inverted
        if (status == LED_OFF) {
            digitalWrite(LEDPIN, LOW);
        }
        else {
            digitalWrite(LEDPIN, HIGH);
        }
        //
        break;
    }
}
// ____________________________________________________________________________________________________________________
//  Name:   get_LEDStatusString()
//  Usage:  get the LED status as a String
//  Input:  none
//  Return: String of LED Status
// ____________________________________________________________________________________________________________________
void getLEDStatus(char* status, size_t statussize) {
    switch (config.ledStatus) {
    case LEDPIN_STATUS_OFF:
        snprintf(status, statussize, "OFF");
        break;
    case LEDPIN_STATUS_ON:
        snprintf(status, statussize, "ON");
        break;
    case LEDPIN_STATUS_FOLLOW:
        snprintf(status, statussize, "FOLLOW");
        break;
    case LEDPIN_STATUS_INVERTED:
        snprintf(status, statussize, "INVERTED");
        break;
    default:
        snprintf(status, statussize, "UNKNOWN");
    }
}

// ____________________________________________________________________________________________________________________
//  Name:   int getFullOnTime(void) 
//  Usage:  get the delay time for the wait in milliseconds to get to the give time in seconds for full light on.
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________
void setFullOnTime(void) {
    delayTime = settings.fullLightOnTimeSeconds * 1000 / 255;
    // FIXME     delayTime = settings.fullLightOnTimeSeconds * 1000 / LIGHT_FULL_ON;
    // value in seconds multiplied with 1000 to get to millseconds, devided by the amount of steps to take.
    // 900 *1000 seconds /255 steps = 3529
}

// ____________________________________________________________________________________________________________________
//  Name:   void setupDevice()
//  Usage:  setup the device
//  Input:  none
//  Return: none
// ____________________________________________________________________________________________________________________
void setupDevice() {
    // set the 8266 ports modes
    // Builtin LED is inverted.
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(LEDPIN, OUTPUT);
    pinMode(LEDSTRIPPIN1, OUTPUT);
    pinMode(LEDSTRIPPIN2, OUTPUT);
    pinMode(TIMERDETECTPIN, INPUT);
    // Builtin LED is inverted.
    digitalWrite(LED_BUILTIN, HIGH);
    // setup the ONBOARD LED
    //setLED(LED_ON);
    // load the settings
    loadSettings(fileNameSettings, settings);

    // setup the 230V-Timer INput on a debounce 
    voltageTimer = digitalRead(TIMERDETECTPIN);
    voltageTimerBounce.attach(TIMERDETECTPIN, INPUT);
    voltageTimerBounce.interval(25);
    voltageTimerBounce.update();
    voltageTimer = voltageTimerBounce.read();

    // start up values
    ledBrightness = 0;
    setFullOnTime();
    state = LIGHT_OFF;
    lastState = LIGHT_OFF;
    // let's check the Timer input at startup.
    voltageTimerBounce.update();
    voltageTimer = voltageTimerBounce.read();

    DEBUG_PRINT("voltagetimer\n");
    DEBUG_PRINT(voltageTimer);
    if (voltageTimer) {
        state = LIGHT_FADE_OUT;
    }
    else {
        state = LIGHT_FADE_IN;
    }

}

// ____________________________________________________________________________________________________________________
//  Name:   loopDevice
//  Usage:  
//  Input:  wifistatus, if wifi and mqtt is ok, this is true, else false
//  Return: none
// ____________________________________________________________________________________________________________________
void loopDevice(bool wifiStatus) {

    // state machine
    // LIGHT OFF
    // LIGTH ON
    // LIGHT FADE IN
    // LIGHT FADE OUT
    // WAIT, to set the time between full on and off or visa versa 

    voltageTimerBounce.update();
    // check if it is changed.
    if (voltageTimerBounce.changed()) {
        voltageTimer = voltageTimerBounce.read();
        DEBUG_PRINT("voltagetimer\n");
        DEBUG_PRINT(voltageTimer);

        if (voltageTimer) {
            state = LIGHT_FADE_OUT;
        }
        else {
            state = LIGHT_FADE_IN;
        }
    }
    // state machine light 

    switch (state) {
    case LIGHT_ON:
        analogWrite(LEDSTRIPPIN1, LIGHT_FULL_ON);
        analogWrite(LEDSTRIPPIN2, LIGHT_FULL_ON);
        lastState = LIGHT_ON;
        DEBUG_PRINT("LIGHT ON\n");
        DEBUG_PRINT(ledBrightness);
        digitalWrite(LED_BUILTIN, LOW);
        digitalWrite(LEDPIN, LOW);
        break;
    case LIGHT_OFF:
        analogWrite(LEDSTRIPPIN1, LIGHT_FULL_OFF);
        analogWrite(LEDSTRIPPIN2, LIGHT_FULL_OFF);
        // check on detection timer
        DEBUG_PRINT("LIGHT OFF\n");
        DEBUG_PRINT(ledBrightness);
        lastState = LIGHT_OFF;
        digitalWrite(LED_BUILTIN, HIGH);
        digitalWrite(LEDPIN, LOW);
        break;
    case LIGHT_FADE_IN:
        analogWrite(LEDSTRIPPIN1, ledBrightness);
        analogWrite(LEDSTRIPPIN2, ledBrightness);
        timeLastTransition = millis();
        if (ledBrightness == (LIGHT_FULL_ON)) {
            state = LIGHT_ON;
        }
        else {
            lastState = LIGHT_FADE_IN;
            state = WAIT_DELAY;
        }
        digitalWrite(LEDPIN, LOW);
        DEBUG_PRINT("LIGHT FADE IN\n");
        DEBUG_PRINT(ledBrightness);
        break;
    case LIGHT_FADE_OUT:
        analogWrite(LEDSTRIPPIN1, ledBrightness);
        analogWrite(LEDSTRIPPIN2, ledBrightness);
        timeLastTransition = millis();
        if (ledBrightness == (LIGHT_FULL_OFF)) {
            state = LIGHT_OFF;
        }
        else {
            lastState = LIGHT_FADE_OUT;
            state = WAIT_DELAY;
        }
        digitalWrite(LEDPIN, LOW);
        DEBUG_PRINT("LIGHT FADE OUT\n");
        DEBUG_PRINT(ledBrightness);
        break;
    case WAIT_DELAY:
        if (millis() - timeLastTransition >= delayTime) {
            DEBUG_PRINT("WAIT.....\n");
            DEBUG_PRINT(".....\n");
            if (lastState == LIGHT_FADE_IN && ledBrightness < LIGHT_FULL_ON) {
                ledBrightness++;
                digitalWrite(LEDPIN, HIGH);
                state = LIGHT_FADE_IN;
            }
            else if (lastState == LIGHT_FADE_OUT && ledBrightness > LIGHT_FULL_OFF) {
                ledBrightness--;
                digitalWrite(LEDPIN, HIGH);
                state = LIGHT_FADE_OUT;
            }
        }
        break;
    default:
        state = LIGHT_OFF;
        break;
    }

}

#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager

//#include "MQTT.h"
#include "configuration.h"
#include "debugger.h"
#include "device.h"  // FIXME with a get/set? here for resetFlag
#include "secrets.h"

// ____________________________________________________________________________________________________________________
//  Name:   WiFiConnection
//  Usage:  Setup the WiFi and load of write the config settings
//  Input:  none
//  Return: bool, true if all okay.
// ____________________________________________________________________________________________________________________
boolean WiFiConnection() {
	//resetFlag false, do this
	IPAddress IPlocal, IPgateway, IPsubnet, IPDNSprimary, IPDNSsecondary;
	if (!IPlocal.fromString(config.IPlocal)) {
		DEBUG_PRINT("not a valid local IP");
		DEBUG_PRINT(config.IPlocal);
	}
	else {
		DEBUG_PRINT("local IP");
		DEBUG_PRINT(IPlocal);
	}
	if (!IPgateway.fromString(config.IPgateway)) {
		DEBUG_PRINT("not a valid IP gateway");
		DEBUG_PRINT(config.IPgateway);
	}
	else {
		DEBUG_PRINT("IP gateway");
		DEBUG_PRINT(IPgateway);
	}
	if (!IPsubnet.fromString(config.IPsubnet)) {
		DEBUG_PRINT("not a valid IP subnet");
		DEBUG_PRINT(config.IPsubnet);
	}
	else {
		DEBUG_PRINT("IP subnet");
		DEBUG_PRINT(IPsubnet);
	}
	if (!IPDNSprimary.fromString(config.IPDNSprimary)) {
		DEBUG_PRINT("not a valid IPDNSprimary");
		DEBUG_PRINT(config.IPDNSprimary);
	}
	else {
		DEBUG_PRINT("IP primary DNS");
		DEBUG_PRINT(IPDNSprimary);
	}
	if (!IPDNSsecondary.fromString(config.IPDNSsecondary)) {
		DEBUG_PRINT("not a valid IPDNSecondary");
		DEBUG_PRINT(config.IPDNSsecondary);
	}
	else {
		DEBUG_PRINT("IP secondary DNS");
		DEBUG_PRINT(IPDNSsecondary);
	}

	if (!WiFi.config(IPlocal, IPgateway, IPsubnet, IPDNSprimary, IPDNSsecondary)) {
		Serial.println("STA Failed to configure");
	}

	DEBUG_PRINT("starting wifi connection");
	WiFi.mode(WIFI_STA);
	unsigned long startedAt = millis();
	// org      WiFi.begin();
	WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS);  // Start the access point

	while (WiFi.status() != WL_CONNECTED) {
		if (millis() - startedAt > 5000) {
			DEBUG_PRINT("WiFi connection FAILED!!\r\n");
			return false;
		}  // not there after 5 seconds, let's kill it
		yield();
	}
	DEBUG_PRINT("");  // WiFi info
	DEBUG_PRINT("WiFi connected to:");
	DEBUG_PRINT(WiFi.SSID());
	DEBUG_PRINT("IP address: ");
	DEBUG_PRINT(WiFi.localIP());
	DEBUG_PRINT("Connected");
	return true;
}

// ____________________________________________________________________________________________________________________
//  Name:   checkWiFiconnected()
//  Usage:  check if the MQTT needs to reconnect
//  Input:  none
//  Return: bool true if needed
// ____________________________________________________________________________________________________________________
bool checkWiFiconnected() {
	if (WiFi.status() != WL_CONNECTED) {
		DEBUG_PRINT("OH NO! WiFi failed! reconnecting to WiFi...\r\n");
		WiFi.disconnect();
		return WiFiConnection();
	}
	else {
		//        DEBUG_PRINT("WiFi connected\r\n");
		return true;
	}
}

// ____________________________________________________________________________________________________________________
//  Name:   get_LEDStatusString()
//  Usage:  get the LED status as a String
//  Input:  none
//  Return: String of LED Status
// ____________________________________________________________________________________________________________________
void getWifiStatus(char* status, size_t statussize) {
	bool wifiStat = false;
	wifiStat = checkWiFiconnected();
	if (wifiStat) {
		snprintf(status, statussize, "ON");
	}
	else {
		snprintf(status, statussize, "OFF");
	}
}

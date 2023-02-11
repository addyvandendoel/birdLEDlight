
#include <WiFiManager.h>  //https://github.com/tzapu/WiFiManager

//#include "MQTT.h"
#include "configuration.h"
#include "debugger.h"
#include "device.h"  // FIXME with a get/set? here for resetFlag
#include "secrets.h"

uint8_t max_connections = 4;//Maximum Connection Limit for AP
int current_stations = 0, new_stations = 0;

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


	WiFi.softAPConfig(IPlocal, IPgateway, IPsubnet);
	if (WiFi.softAP(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS, 1, false, max_connections))
	{
		Serial.print("Access Point is Creadted with SSID: ");
		Serial.println(CONFIG_WIFI_SSID);
		Serial.print("Max Connections Allowed: ");
		Serial.println(max_connections);
		Serial.print("Access Point IP: ");
		Serial.println(WiFi.softAPIP());
		return true;
	}
	else
	{
		Serial.println("Unable to Create Access Point");
		return false;
	}
	return false;
}

// ____________________________________________________________________________________________________________________
//  Name:   checkWiFiconnected()
//  Usage:  check if the MQTT needs to reconnect
//  Input:  none
//  Return: bool true if needed
// ____________________________________________________________________________________________________________________
void checkWiFiconnected() {
	new_stations = WiFi.softAPgetStationNum();
	struct station_info* stat_info;
	stat_info = wifi_softap_get_station_info();
	struct ip4_addr* IPaddress;
	IPAddress address;
	int cnt = 1;

	if (current_stations < new_stations)//Device is Connected
	{
		current_stations = new_stations;
		Serial.print("New Device Connected to SoftAP... Total Connections: ");
		Serial.println(current_stations);
	}

	if (current_stations > new_stations)//Device is Disconnected
	{
		current_stations = new_stations;
		Serial.print("Device disconnected from SoftAP... Total Connections: ");
		Serial.println(current_stations);
	}

	while (stat_info != NULL)
	{
		IPaddress = &stat_info->ip;
		address = IPaddress->addr;

		Serial.print(cnt);
		Serial.print(": IP: ");
		Serial.print((address));
		Serial.print(" MAC: ");

		uint8_t* p = stat_info->bssid;
		Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X", p[0], p[1], p[2], p[3], p[4], p[5]);

		stat_info = STAILQ_NEXT(stat_info, next);
		cnt++;
		Serial.println();

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
	snprintf(status, statussize, "ON");
	//	wifiStat = checkWiFiconnected();
	//	if (wifiStat) {
	//		snprintf(status, statussize, "ON");
	//	}
	//	else {
	//		snprintf(status, statussize, "OFF");
	//	}

}

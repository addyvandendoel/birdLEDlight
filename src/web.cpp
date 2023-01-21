
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <LittleFS.h>
#include "configuration.h"
#include "debugger.h"
#include "device.h"
#include "wifi.h"
#include "settings.h"
//String sliderValue;

char waitTime1[11];
// String waitTime = "800";
// create server
AsyncWebServer server(80);
const char* PARAM_MESSAGE = "message";

// page not found exit
void notFound(AsyncWebServerRequest* request) {
	request->send(404, "text/plain", "Not found");
}

/// process requests
String processor(const String& var) {
	Serial.println(var);
	if (var == "NAME") {
		return config.deviceName;
	}
	else if (var == "FRIENDLYNAME") {
		return config.deviceNameFriendly;
	}
	else if (var == "IPADDRESS") {
		return config.IPlocal;
	}
	else if (var == "IPGATEWAY") {
		return config.IPgateway;
	}
	else if (var == "IPSUBNET") {
		return config.IPsubnet;
	}
	else if (var == "LEDSTATUS") {
		char status[10] = { 0 };
		getLEDStatus(status, sizeof status);
		return (String)status;
	}
	else if (var == "FIRMWAREVERSION") {
		return config.sw_version;
	}
	else if (var == "WIFISTATUS") {
		char status[5] = { 0 };
		getWifiStatus(status, sizeof status);
		return (String)status;
	}
	else if (var == "SLIDERVALUE") {
		String sliderValue = String(ledBrightness);
		return (String)sliderValue;
	}
	else if (var == "WAITTIME") {
		snprintf(waitTime1, sizeof(waitTime1), "%lu", (unsigned long)settings.fullLightOnTimeSeconds);
		return (String)waitTime1;
	}
	return String();
	String a = "FAILED";
	return a;
}

void setupWebserver() {
	DEBUG_PRINT("setupWebServer......\n");
	//    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
	//    request->send(200, "text/plain", "Hello, world"); });
	server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(LittleFS, "/config.html", String(), false, processor);
		});
	// Send a GET request to <IP>/get?message=<message>
	server.on("/get", HTTP_GET, [](AsyncWebServerRequest* request) {
		String message;
	if (request->hasParam(PARAM_MESSAGE)) {
		message = request->getParam(PARAM_MESSAGE)->value();
	}
	else {
		message = "No message sent get";
	}
	request->send(200, "text/plain", "Hello, GET: " + message);
		});

	// Send a POST request to <IP>/post with a form field message set to
	// <message>
	server.on("/post", HTTP_POST, [](AsyncWebServerRequest* request) {
		String message;
	if (request->hasParam(PARAM_MESSAGE, true)) {
		message = request->getParam(PARAM_MESSAGE, true)->value();
	}
	else {
		message = "No message sent post";
	}
	request->send(200, "text/plain", "Hello, POST: " + message);
		});



	server.on("/slider", HTTP_GET, [](AsyncWebServerRequest* request) {
		String inputMessage;
	// GET input1 value on <ESP_IP>/slider?value=<inputMessage>
	if (request->hasParam("value")) {
		inputMessage = request->getParam("value")->value();
		// sliderValue = inputMessage;
		// set ledBrightness to the value received.
		ledBrightness = inputMessage.toInt();
	}
	else {
		inputMessage = "No message sent slider";
	}
	Serial.println(inputMessage);
	request->send(200, "text/plain", "OK");
		});



	server.on("/waittime", HTTP_GET, [](AsyncWebServerRequest* request) {
		String inputMessage;
	// GET input1 value on <ESP_IP>/waittime?value=<inputMessage>
	if (request->hasParam("value")) {
		inputMessage = request->getParam("value")->value();
		// set ledBrightness to the value received.
		settings.fullLightOnTimeSeconds = inputMessage.toInt();
		// save the settings
		saveSettings(fileNameSettings, settings);
		// activate the new time
		setFullOnTime();
	}
	else {
		inputMessage = "No message sent waittimer";
	}
	Serial.println(inputMessage);
	request->send(200, "text/plain", "OK");
		});



	server.on("/config", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(LittleFS, "/config.html", String(), false, processor);
		});
	server.on("/device", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(LittleFS, "/device.html", String(), false, processor);
		});
	server.on("/util", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(LittleFS, "/util.html", String(), false, processor);
		});

	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send(LittleFS, "/style.css", "text/css");
		});

	server.on("/configName", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send_P(200, "text/plain", config.deviceName);
		});

	server.on("/configFriendlyName", HTTP_GET,
		[](AsyncWebServerRequest* request) {
			request->send_P(200, "text/plain", config.deviceNameFriendly);
		});
	server.on("/configIPAddress", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send_P(200, "text/plain", config.IPlocal);
		});
	server.on("/configIPGateway", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send_P(200, "text/plain", config.IPgateway);
		});
	server.on("/configIPSubnet", HTTP_GET, [](AsyncWebServerRequest* request) {
		request->send_P(200, "text/plain", config.IPsubnet);
		});
	server.on("/configLEDStatus", HTTP_GET, [](AsyncWebServerRequest* request) {
		char status[10] = { 0 };
	getLEDStatus(status, sizeof status);
	request->send_P(200, "text/plain", status);
		});
	server.on("/configFirmwareVersion", HTTP_GET,
		[](AsyncWebServerRequest* request) {
			request->send_P(200, "text/plain", config.sw_version);
		});

	server.on("/wifistatus", HTTP_GET, [](AsyncWebServerRequest* request) {
		char status[5] = { 0 };
	getWifiStatus(status, sizeof status);
	request->send_P(200, "text/plain", status);
		});
	server.on("/devicerestart", HTTP_GET, [](AsyncWebServerRequest* request) {
		delay(2000);
	ESP.restart();
	request->send_P(200, "text/plain", "OK");
		});
	server.on("/devicereset", HTTP_GET, [](AsyncWebServerRequest* request) {
		delay(2000);
	ESP.reset();
	request->send_P(200, "text/plain", "OK");
		});

	server.onNotFound(notFound);

	server.begin();
}

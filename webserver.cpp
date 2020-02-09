/*
 * webserver.c
 *
 *  Created on: Feb 9, 2020
 *      Author: tsugua
 */

#include "webserver.h"
#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#else
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char* CURRENT_PASSWORD_INPUT = "input_current_password";
const char* SSID_INPUT = "input_ssid";
const char* PASSWORD_INPUT = "input_password";

extern String changeWifi(const char* currentPassword, const char* newSSID, const char* newPassword);

// HTML web page to handle 3 input fields (input1, input2, input3)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
	<title>LED Wifi settings</title>
	<style>
		form  { display: table;      }
		p     { display: table-row;  }
		label { display: table-cell; }
		input { display: table-cell; }
	</style>
</head>

<body>
	<form action="/get" autocomplete="off">
		<p>
			<label for="current_password">Current password: &nbsp</label>
			<input name="input_current_password" type="text">
		</p>
		<p>
			<label for="input_ssid">New SSID: </label>
			<input name="input_ssid" type="text">
		</p>
		<p>
			<label for="input_password">New password: &nbsp </label>
			<input name="input_password" type="text">
		</p>
		<br>
		<p>
			<input type="submit" value="Set">
		</p>
	</form>
</body>

</html>
)rawliteral";

void notFound(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "Not found");
}


void webserver_init(){
	// Send web page with input fields to client
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send_P(200, "text/html", index_html);
	});

	server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
		String current_password;
		String new_ssid;
		String new_password;
		// GET currentPassowrd value on <ESP_IP>/get?input_current_password=<inputMessage>
		if (request->hasParam(CURRENT_PASSWORD_INPUT)) {
			current_password = request->getParam(CURRENT_PASSWORD_INPUT)->value();
		}
		// GET newnew  SSID value on <ESP_IP>/get?input_ssid=<inputMessage>
		if (request->hasParam(SSID_INPUT)) {
			new_ssid = request->getParam(SSID_INPUT)->value();
		}
		// GET new password value on <ESP_IP>/get?input_password=<inputMessage>
		if (request->hasParam(PASSWORD_INPUT)) {
			new_password = request->getParam(PASSWORD_INPUT)->value();
		}
		String reply = changeWifi((current_password != NULL?current_password.c_str():""),
				(new_ssid != NULL?new_ssid.c_str():""), (new_password != NULL?new_password.c_str():""));
		request->send(200, "text/html", reply +
				"<br><a href=\"/\">Return</a>");
	});
	server.onNotFound(notFound);
	server.begin();
}

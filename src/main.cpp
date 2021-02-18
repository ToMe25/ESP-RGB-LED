/*
 * main.cpp
 *
 *  Created on: 06.11.2020
 *      Author: ToMe25
 */

#include <main.h>

void setup() {
	Serial.begin(115200);

	WiFi.mode(WIFI_STA);
	WiFi.disconnect(true);

	if (!WiFi.config(INADDR_NONE, GATEWAY, SUBNET)) {// setting the local ip here causes setHostname to fail.
		Serial.println("Configuring WiFi failed!");
		return;
	}

	WiFi.setHostname(HOSTNAME);

	WiFi.begin(WIFI_SSID, WIFI_PASS);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Establishing a WiFi connection failed!");
		return;
	}

	WiFi.enableIpV6();

	localhost = WiFi.localIP();

	Serial.print("IP Address: ");
	Serial.println(localhost);

	server.init();
}

void loop() {
	light.writeColor();
	delay(100);
}

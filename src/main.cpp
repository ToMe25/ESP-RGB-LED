/*
 * main.cpp
 *
 *  Created on: 06.11.2020
 *      Author: ToMe25
 */

#include <main.h>

uint main::color = 0xffff00;
uint8_t main::rgb[3];

uint main::getColor() {
	return color;
}

void main::setColor(uint color) {
	main::color = color;
	rgb[0] = (color >> 16);
	rgb[1] = (color >> 8);
	rgb[2] = color;
}

uint8_t main::getRed() {
	return rgb[0];
}

uint8_t main::getGreen() {
	return rgb[1];
}

uint8_t main::getBlue() {
	return rgb[2];
}

void setup() {
	Serial.begin(115200);

	ledcSetup(CHANNEL_RED, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_GREEN, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_BLUE, LED_RATE, LED_RESOLUTION);

	ledcAttachPin(PIN_RED, CHANNEL_RED);
	ledcAttachPin(PIN_GREEN, CHANNEL_GREEN);
	ledcAttachPin(PIN_BLUE, CHANNEL_BLUE);

	main::setColor(main::getColor());

	WiFi.mode(WIFI_STA);

	if (!WiFi.config(localhost, GATEWAY, SUBNET)) {
		Serial.println("Configuring WiFi failed!");
		return;
	}

	WiFi.setHostname(HOSTNAME);

	WiFi.begin(SSID, PASSWORD);
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
	ledcWrite(CHANNEL_RED, main::getRed());
	ledcWrite(CHANNEL_GREEN, main::getGreen());
	ledcWrite(CHANNEL_BLUE, main::getBlue());
	delay(100);
}

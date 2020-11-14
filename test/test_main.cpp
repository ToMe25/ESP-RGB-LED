/*
 * testmain.cpp
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#include "test_main.h"
#include <unity.h>

void test_led_color() {
	light.setColor(0x00ff00);
	light.writeColor();
	delay(100);
	TEST_ASSERT_EQUAL(0, ledcRead(0));
	TEST_ASSERT_EQUAL(255, ledcRead(1));
	TEST_ASSERT_EQUAL(0, ledcRead(2));
	TEST_ASSERT_EQUAL(0x00ff00, light.getColor());
}

void test_web_server_input() {
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

	server.init();

	tcp_client = new AsyncClient();

	tcp_client->onError([](void * arg, AsyncClient * client, int error) {
		Serial.println("Connection Error!");
		tcp_client = NULL;
		delete client;
	}, NULL);

	tcp_client->onConnect([](void * arg, AsyncClient * client) {
		Serial.println("Connected");
		tcp_client->onError(NULL, NULL);

		client->onDisconnect([](void * arg, AsyncClient * c) {
			Serial.println("Disconnected");
			tcp_client = NULL;
			delete c;
		}, NULL);

		client->onData([](void * arg, AsyncClient * c, void * data, size_t len) {
			Serial.println("Received response.");
			std::string website = (char*) data;
			TEST_ASSERT_NOT_EQUAL(std::string::npos, website.find("#ff0000"));
		}, NULL);

		const std::string request =
				"POST / HTTP/1.1\r\n"
				"Host: localhost\r\n"
				"Content-Type: application/x-www-form-urlencoded\r\n"
				"Content-Length: 15\r\n\r\n"
				"color=%23ff0000";

		client->write(request.c_str());
	}, NULL);

	if (!tcp_client->connect(WiFi.localIP(), 80)) {
		Serial.println("Connection Failed!");
	}

	delay(500);
	light.writeColor();
	TEST_ASSERT_EQUAL(0xff0000, light.getColor());
}

void setup() {
	delay(2000);
	Serial.begin(9600);
	UNITY_BEGIN();

	RUN_TEST(test_led_color);

	RUN_TEST(test_web_server_input);

	UNITY_END();
}

void loop() {

}

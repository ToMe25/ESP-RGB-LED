/*
 * test_main.cpp
 *
 * Created on: 14.11.2020
 *
 * Copyright 2021 ToMe25
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "test_main.h"
#include <unity.h>

void test_led_color() {
	light.color1 = 0x00ff00;
	light.writeColor();
	delay(100);
	TEST_ASSERT_EQUAL(0, ledcRead(0));
	TEST_ASSERT_EQUAL(255, ledcRead(1));
	TEST_ASSERT_EQUAL(0, ledcRead(2));
	TEST_ASSERT_EQUAL(0x00ff00, light.getColor());
}

void initWiFi() {
	WiFi.softAP(SSID, PASSPHRASE);

	localhost = WiFi.softAPIP();

	server.init();
}

std::string request(const std::string request) {
	response = "";

	if (!tcp_client.connect(localhost, 80)) {
		Serial.println("Connection Failed!");
	    return response;
	}

	if (tcp_client.print(request.c_str()) > 0) {
		while (tcp_client.connected() && tcp_client.available() == 0) {
			delay(10);
		}

		while (tcp_client.available()) {
			response += tcp_client.read();
		}
		if (tcp_client.connected()) {
			tcp_client.stop();
		}
	} else {
		tcp_client.stop();
		Serial.println("Send Failed");
		while (tcp_client.connected()) {
			delay(10);
		}
	}

	return response;
}

void test_web_server_input() {
	const std::string requestStr = "POST /index.html HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"Connection: close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: 15\r\n\r\n"
			"color=%23ff0000";

	const std::string response = request(requestStr);

	TEST_ASSERT_NOT_EQUAL(std::string::npos, response.find("#ff0000"));

	light.writeColor();
	TEST_ASSERT_EQUAL(0xff0000, light.getColor());
}

void test_web_server_dual_color_input() {
	const std::string requestStr = "POST /index.html HTTP/1.1\r\n"
			"Host: localhost\r\n"
			"Connection: close\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: 78\r\n\r\n"
			"color=%2300ff00&fade=1&time=3&color2=%230000ff&fade2=0.5&time2=5&submit=double";

	const std::string response = request(requestStr);

	TEST_ASSERT_NOT_EQUAL(std::string::npos, response.find("#00ff00"));
	TEST_ASSERT_NOT_EQUAL(std::string::npos, response.find("#0000ff"));

	light.writeColor();
	TEST_ASSERT_EQUAL(0x00ff00, light.getColor());
	TEST_ASSERT_EQUAL(0x00ff00, light.color1);
	TEST_ASSERT_EQUAL(0x0000ff, light.color2);
	TEST_ASSERT_EQUAL(1000, light.fade1);
	TEST_ASSERT_EQUAL(500, light.fade2);
	TEST_ASSERT_EQUAL(4000, light.time1);
	TEST_ASSERT_EQUAL(5500, light.time2);

	delay(4200);
	light.writeColor();
	TEST_ASSERT_EQUAL(0x0000ff, light.getColor());
}

void setup() {
	delay(2000);
	Serial.begin(9600);
	UNITY_BEGIN();

	RUN_TEST(test_led_color);

	initWiFi();

	RUN_TEST(test_web_server_input);

	RUN_TEST(test_web_server_dual_color_input);

	WiFi.softAPdisconnect(true);

	UNITY_END();
}

void loop() {

}

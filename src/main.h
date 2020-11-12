/*
 * main.h
 *
 *  Created on: 12.11.2020
 *      Author: ToMe25
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <Arduino.h>
#include <WebserverHandler.h>

static const uint8_t PIN_RED = 5;
static const uint8_t PIN_GREEN = 18;
static const uint8_t PIN_BLUE = 19;

static const uint8_t CHANNEL_RED = 0;
static const uint8_t CHANNEL_GREEN = 1;
static const uint8_t CHANNEL_BLUE = 2;

static constexpr char SSID[] = "WIFI_SSID";
static constexpr char PASSWORD[] = "WPA_PASSPHRASE";

static IPAddress localhost(192, 168, 2, 120);
static const IPAddress GATEWAY(192, 168, 2, 1);
static const IPAddress SUBNET(255, 255, 0, 0);

static constexpr char HOSTNAME[] = "lightcontrol";

static const uint8_t LED_RESOLUTION = 8;
static const uint16_t LED_RATE = 5000;

static WebserverHandler server;

class main {
public:
	static uint getColor();
	static void setColor(uint color);

	static uint8_t getRed();
	static uint8_t getGreen();
	static uint8_t getBlue();

private:
	static uint color;
	static uint8_t rgb[3];
};

#endif /* SRC_MAIN_H_ */

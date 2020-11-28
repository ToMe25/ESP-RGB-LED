/*
 * LedHandler.h
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#ifndef LIB_LEDHANDLER_LEDHANDLER_H_
#define LIB_LEDHANDLER_LEDHANDLER_H_

#include <Arduino.h>
#include <chrono>

class LedHandler {
public:
	LedHandler();
	virtual ~LedHandler();

	void writeColor();

	uint getColor();

	uint color1 = 0xffff00;
	uint16_t fade1 = 0;
	uint time1 = 10000;

	uint color2 = 0x00ffff;
	uint16_t fade2 = 0;
	uint time2 = 10000;

	bool dual_color = false;

private:
	uint color = 0;
	uint8_t rgb[3];

	bool last_dual_color = false;

	std::chrono::steady_clock::time_point last_color_change;

	void setColor(uint new_color, uint16_t fade);

	static const uint8_t PIN_RED = 5;
	static const uint8_t PIN_GREEN = 18;
	static const uint8_t PIN_BLUE = 19;

	static const uint8_t CHANNEL_RED = 0;
	static const uint8_t CHANNEL_GREEN = 1;
	static const uint8_t CHANNEL_BLUE = 2;

	static const uint8_t LED_RESOLUTION = 8;
	static const uint16_t LED_RATE = 5000;
};

#endif

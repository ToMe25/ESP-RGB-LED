/*
 * LedHandler.h
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#ifndef LIB_LEDHANDLER_LEDHANDLER_H_
#define LIB_LEDHANDLER_LEDHANDLER_H_

#include <Arduino.h>

class LedHandler {
public:
	LedHandler();
	virtual ~LedHandler();

	uint getColor();
	void setColor(uint new_color);

	void writeColor();

private:
	uint color = 0xffff00;
	uint8_t rgb[3];

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

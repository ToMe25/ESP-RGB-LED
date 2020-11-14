/*
 * LedHandler.cpp
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#include "LedHandler.h"

LedHandler::LedHandler() {
	rgb[0] = (color >> 16);
	rgb[1] = (color >> 8);
	rgb[2] = color;

	ledcSetup(CHANNEL_RED, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_GREEN, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_BLUE, LED_RATE, LED_RESOLUTION);

	ledcAttachPin(PIN_RED, CHANNEL_RED);
	ledcAttachPin(PIN_GREEN, CHANNEL_GREEN);
	ledcAttachPin(PIN_BLUE, CHANNEL_BLUE);
}

LedHandler::~LedHandler() {

}

uint LedHandler::getColor() {
	return color;
}

void LedHandler::setColor(uint new_color) {
	color = new_color;
	rgb[0] = (color >> 16);
	rgb[1] = (color >> 8);
	rgb[2] = color;
}

void LedHandler::writeColor() {
	ledcWrite(CHANNEL_RED, rgb[0]);
	ledcWrite(CHANNEL_GREEN, rgb[1]);
	ledcWrite(CHANNEL_BLUE, rgb[2]);
}

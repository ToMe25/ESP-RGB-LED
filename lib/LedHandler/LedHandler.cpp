/*
 * LedHandler.cpp
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#include "LedHandler.h"
#include "driver/ledc.h"

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

	ledc_fade_func_install(LEDC_FADE_NO_WAIT);
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
	if (fade > 10000) {
		fade = 10000;
	} else if (fade < 0) {
		fade = 0;
	}
	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_RED), rgb[0], fade, LEDC_FADE_NO_WAIT);
	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_GREEN), rgb[1], fade, LEDC_FADE_NO_WAIT);
	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_BLUE), rgb[2], fade, LEDC_FADE_NO_WAIT);
}

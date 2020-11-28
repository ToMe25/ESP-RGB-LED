/*
 * LedHandler.cpp
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#include "LedHandler.h"
#include "driver/ledc.h"

using namespace std::chrono;

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

void LedHandler::setColor(uint new_color, uint16_t fade) {
	color = new_color;
	rgb[0] = (color >> 16);
	rgb[1] = (color >> 8);
	rgb[2] = color;

	if (fade1 > 10000) {
		fade1 = 10000;
	} else if (fade1 < 0) {
		fade1 = 0;
	}

	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_RED), rgb[0], fade, LEDC_FADE_NO_WAIT);
	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_GREEN), rgb[1], fade, LEDC_FADE_NO_WAIT);
	ledc_set_fade_time_and_start(LEDC_HIGH_SPEED_MODE, ledc_channel_t(CHANNEL_BLUE), rgb[2], fade, LEDC_FADE_NO_WAIT);
}

uint LedHandler::getColor() {
	return color;
}

void LedHandler::writeColor() {
	if (color != color1 && (!dual_color || color != color2)) {
		setColor(color1, fade1);
		if (dual_color) {
			last_color_change = steady_clock::now();
			last_dual_color = true;
		}
	} else if (dual_color) {
		steady_clock::time_point now = steady_clock::now();

		if (!last_dual_color) {
			last_color_change = now;
			last_dual_color = true;
		}

		if (color == color1 && duration_cast<milliseconds>(now - last_color_change).count() > time1) {
			setColor(color2, fade2);
			last_color_change = now;
		} else if (color == color2 && duration_cast<milliseconds>(now - last_color_change).count() > time2) {
			setColor(color1, fade1);
			last_color_change = now;
		}
	} else {
		last_dual_color = false;
	}
}

/*
 * main.h
 *
 *  Created on: 12.11.2020
 *      Author: ToMe25
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <Arduino.h>
#include <LedHandler.h>
#include <WebserverHandler.h>

// includes the content of the file "wifissid.txt" in the project root.
// Make sure this file doesn't end with an empty line.
extern const char WIFI_SSID[] asm("_binary_wifissid_txt_start");
// includes the content of the file "wifipass.txt" in the project root.
// Make sure this file doesn't end with an empty line.
extern const char WIFI_PASS[] asm("_binary_wifipass_txt_start");

static IPAddress localhost(192, 168, 2, 120);
static const IPAddress GATEWAY(192, 168, 2, 1);
static const IPAddress SUBNET(255, 255, 0, 0);

static constexpr char HOSTNAME[] = "esp-rgb-led";

static const bool DEFAULT_REQUIRE_LOGIN = true;

static LedHandler light;

static WebserverHandler server(light, DEFAULT_REQUIRE_LOGIN);

#endif

/*
 * testmain.h
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#ifndef TEST_TEST_MAIN_H_
#define TEST_TEST_MAIN_H_

#include <Arduino.h>
#include <AsyncTCP.h>
#include <LedHandler.h>
#include <WebserverHandler.h>

static constexpr char SSID[] = "WIFI_SSID";
static constexpr char PASSWORD[] = "WPA_PASSPHRASE";

static IPAddress localhost(192, 168, 2, 120);
static const IPAddress GATEWAY(192, 168, 2, 1);
static const IPAddress SUBNET(255, 255, 0, 0);

static constexpr char HOSTNAME[] = "lightcontrol";

static const bool DEFAULT_REQUIRE_LOGIN = false;

static LedHandler light;

static WebserverHandler server(light, DEFAULT_REQUIRE_LOGIN);

static AsyncClient *tcp_client = NULL;

#endif

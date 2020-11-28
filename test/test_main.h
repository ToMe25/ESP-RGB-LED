/*
 * testmain.h
 *
 *  Created on: 14.11.2020
 *      Author: ToMe25
 */

#ifndef TEST_TEST_MAIN_H_
#define TEST_TEST_MAIN_H_

#include <Arduino.h>
#include <LedHandler.h>
#include <WebserverHandler.h>

static const char *SSID = "UnitTestAP";
static const char *PASSPHRASE = "MeantForAutomatedTestsOnly";

static IPAddress localhost;

static LedHandler light;

static WebserverHandler server(light, false);

static WiFiClient tcp_client;

static std::string response;

#endif

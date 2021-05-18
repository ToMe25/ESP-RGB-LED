/*
 * test_main.h
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

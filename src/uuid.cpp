/*
 * uuid.cpp
 *
 *  Created on: 12.11.2020
 *      Author: ToMe25
 */

#include <uuid.h>
#include <sstream>
#include <Arduino.h>

using namespace uuid;
using namespace std;

string uuid::generate_uuid() {
	stringstream ss;
	int i;
	ss << hex;
	for (i = 0; i < 8; i++) {
		ss << random(0, 15);
	}
	ss << "-";
	for (i = 0; i < 4; i++) {
		ss << random(0, 15);
	}
	ss << "-4";
	for (i = 0; i < 3; i++) {
		ss << random(0, 15);
	}
	ss << "-";
	ss << random(8, 11);
	for (i = 0; i < 3; i++) {
		ss << random(0, 15);
	}
	ss << "-";
	for (i = 0; i < 12; i++) {
		ss << random(0, 15);
	};
	return ss.str();
}

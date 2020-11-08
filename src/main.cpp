#include <regex>
#include <sstream>
#include <iomanip>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char *ssid = "WIFI_SSID";
const char *password = "WPA_PASSPHRASE";

IPAddress localhost(192, 168, 2, 120);
const IPAddress GATEWAY(192, 168, 2, 1);
const IPAddress SUBNET(255, 255, 0, 0);

const uint8_t PIN_RED = 5;
const uint8_t PIN_GREEN = 18;
const uint8_t PIN_BLUE = 19;

const uint8_t CHANNEL_RED = 0;
const uint8_t CHANNEL_GREEN = 1;
const uint8_t CHANNEL_BLUE = 2;

const uint8_t LED_RESOLUTION = 8;
const uint16_t LED_RATE = 5000;

const std::string HTML =
		"<!DOCTYPE html>\n<html>"
				"<head><meta charset=\"utf-8\">\n<title>Esp32 Led color control</title>\n"
				"<style>input {\nbackground-color: #d3d3d319;\nmargin: 8px 0;\nborder-radius: 4px;\n"
				"border: 1px solid #ccc;\ntransition: background-color .2s;} input:hover {\n"
				"background-color: #d3d3d388;}</style></head>\n"
				"<body bgcolor=\"$color\"><center><div><form action=\"/\" method=\"post\">\n"
				"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\" "
				"required title=\"Submit hex value\" placeholder=\"$color\"><br>\n"
				"<input type=\"submit\" value=\"Submit\"></form></div></center></body></html>";

const char *HOSTNAME = "lightcontrol";

uint32_t color = 0xffff00;
uint8_t rgb[3];

void on_not_found(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "Not found");
}

void on_get_index(AsyncWebServerRequest *request) {
	std::ostringstream os;
	os << '#';
	os.fill('0');
	os << std::setw(6);
	os << std::hex;
	os << std::right;
	os << color;
	request->send(200, "text/html",
			std::regex_replace(HTML, std::regex("\\$color"), os.str()).c_str());
}

void on_post_index(AsyncWebServerRequest *request) {
	if (request->hasParam("color"), true) {
		std::istringstream is(
				request->getParam("color", true)->value().substring(1).c_str());
		is >> std::hex >> color;
		rgb[0] = (color >> 16);
		rgb[1] = (color >> 8);
		rgb[2] = color;
	} else {
		Serial.println("Received a post request missing the color value.");
	}
	on_get_index(request);
}

void setup() {
	Serial.begin(115200);

	ledcSetup(CHANNEL_RED, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_GREEN, LED_RATE, LED_RESOLUTION);
	ledcSetup(CHANNEL_BLUE, LED_RATE, LED_RESOLUTION);

	ledcAttachPin(PIN_RED, CHANNEL_RED);
	ledcAttachPin(PIN_GREEN, CHANNEL_GREEN);
	ledcAttachPin(PIN_BLUE, CHANNEL_BLUE);

	rgb[0] = (color >> 16);
	rgb[1] = (color >> 8);
	rgb[2] = color;

	WiFi.mode(WIFI_STA);

	if (!WiFi.config(localhost, GATEWAY, SUBNET)) {
		Serial.println("Configuring WiFi failed!");
		return;
	}

	WiFi.setHostname(HOSTNAME);

	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Establishing a WiFi connection failed!");
		return;
	}

	WiFi.enableIpV6();

	localhost = WiFi.localIP();

	Serial.print("IP Address: ");
	Serial.println(localhost);

	server.on("/", HTTP_GET, on_get_index);

	server.on("/", HTTP_POST, on_post_index);

	server.onNotFound(on_not_found);

	server.begin();
}

void loop() {
	ledcWrite(CHANNEL_RED, rgb[0]);
	ledcWrite(CHANNEL_GREEN, rgb[1]);
	ledcWrite(CHANNEL_BLUE, rgb[2]);
	delay(100);
}

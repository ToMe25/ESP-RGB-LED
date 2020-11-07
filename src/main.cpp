#include <regex>
#include <sstream>
#include <iomanip>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

const char *ssid = "WIFI_SSID";
const char *password = "WPA_PASSPHRASE";

const std::string HTML =
		"<!DOCTYPE html>\n<html>"
				"<head><meta charset=\"utf-8\">\n<title>Esp32 Led color control</title>\n"
				"<style>input {\nbackground-color: #d3d3d319;\nmargin: 8px 0;\nborder-radius: 4px;\n"
				"border: 1px solid #ccc;\ntransition: background-color .2s;} input[type=range] {\n"
				"-webkit-appearance: none;} input:hover {\nbackground-color: #d3d3d388;}</style></head>\n"
				"<body bgcolor=\"$color\"><center><div><form action=\"/\" method=\"post\">"
				"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\" "
				"required title=\"Submit hex value\" placeholder=\"$color\"><br>\n"
				"<input type=\"range\" name=\"opacity\" id=\"opacity\" required title=\"Submit opacity\" "
				"value=\"$opacity\" min=\"0\" max=\"255\"><br>\n"
				"<input type=\"submit\" value=\"Submit\"></form></div></center></body></html>";

uint32_t color = 0xffff00ff;

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
	os << (color >> 8);
	std::string response = std::regex_replace(HTML, std::regex("\\$color"), os.str());
	os.str("");
	os.clear();
	os << std::dec;
	os << (color & 0x000000FF);
	response = std::regex_replace(response, std::regex("\\$opacity"), os.str());
	request->send(200, "text/html", response.c_str());
}

void on_post_index(AsyncWebServerRequest *request) {
	if (request->hasParam("color"), true) {
		uint32_t col;
		std::istringstream is(
				request->getParam("color", true)->value().substring(1).c_str());
		is >> std::hex >> col;
		color = (color & 0x000000FF) | (col << 8);
	} else {
		Serial.println("Received a post request missing the color value.");
	}
	if (request->hasParam("opacity"), true) {
		uint16_t opacity;
		std::istringstream is(
				request->getParam("opacity", true)->value().c_str());
		is >> opacity;
		color = (color & 0xFFFFFF00) | opacity;
	} else {
		Serial.println("Received a post request missing the opacity value.");
	}
	on_get_index(request);
}

void setup() {
	Serial.begin(115200);

	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		Serial.println("Establishing a WiFi connection failed!");
		return;
	}
	WiFi.enableIpV6();

	server.on("/", HTTP_GET, on_get_index);

	server.on("/", HTTP_POST, on_post_index);

	server.onNotFound(on_not_found);

	server.begin();
}

void loop() {

}
#include <regex>
#include <sstream>
#include <iomanip>
#include <esp_wifi.h>
#include <ESPAsyncWebServer.h>
#include <uuid.h>

const char *ssid = "WIFI_SSID";
const char *password = "WPA_PASSPHRASE";

IPAddress localhost(192, 168, 2, 120);
const IPAddress GATEWAY(192, 168, 2, 1);
const IPAddress SUBNET(255, 255, 0, 0);

const char *HOSTNAME = "lightcontrol";

const uint8_t PIN_RED = 5;
const uint8_t PIN_GREEN = 18;
const uint8_t PIN_BLUE = 19;

const uint8_t CHANNEL_RED = 0;
const uint8_t CHANNEL_GREEN = 1;
const uint8_t CHANNEL_BLUE = 2;

const uint8_t LED_RESOLUTION = 8;
const uint16_t LED_RATE = 5000;

AsyncWebServer server(80);

const std::string MAIN_PAGE =
		"<!DOCTYPE html>\n"
		"<html>\n"
		"<head><meta charset=\"utf-8\">\n"
		"<title>Esp32 Led color control</title>\n"
		"<style>body {\npadding: 0px;\nmargin: 0px;\n}\n"
		"input {\nbackground-color: #d3d3d319;\nmargin: 8px 0;\nborder-radius: 4px;\n"
		"border: 1px solid #ccc;\ntransition: background-color .2s;\n}\n"
		"input:hover {\nbackground-color: #d3d3d388;\n}\n"
		"a {\npadding: 15px;\nmargin: 0px;\ndisplay: inline-block;\n}\n"
		".header {\nbackground-color: #858585;\nmargin: 0px;\nwidth: calc(100% - 8px);\n"
		"border: 4px solid #737373;\ntext-align: right;\n}\n"
		".user {\nbackground-color: #88AEAF;\n}\n"
		".logout {\nbackground-color: #A26B5F;\ncursor: pointer;\n}\n"
		".logout:link {\ncolor: #000000;\n}\n"
		".logout:visited {\ncolor: #000000;\n}\n"
		"[hidden] {\ndisplay: none !important;\n}</style>\n"
		"</head>\n"
		"<body bgcolor=\"$color\"><div class=\"header\"><a class=\"logout\" href=\"$logout\">Logout</a>\n"
		"<a class=\"user\">$user</a></div>\n"
		"<center><form method=\"post\">\n"
		"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\" "
		"required title=\"Submit hex value\" placeholder=\"$color\"><br>\n"
		"<input type=\"submit\" value=\"Submit\">\n"
		"</form></center></body></html>";

const std::string LOGIN_PAGE =
		"<!DOCTYPE html>\n<html>\n"
		"<head><meta charset=\"utf-8\">\n"
		"<title>Login</title>\n"
		"<style>form {\nbackground-color: #f0f0f0;\nborder: 4px solid #d3d3d3;\n"
		"padding: 50px;\npadding-top: 30px;\nborder-radius: 8px;\ndisplay: inline-block;\n}\n"
		"input {\npadding: 12px 20px;\nmargin: 8px 0;\ndisplay: inline-block;\nborder: 1px solid #ccc;\n"
		"border-radius: 20px;\nbox-sizing: border-box;\n}\n"
		"button {\nbackground-color: #4CC550;\ncolor: white;\npadding: 14px 100px;\nmargin: 8px 0;\n"
		"border: none;\nborder-radius: 12px;\ncursor: pointer;\ntransition: all 0.2s;\n"
		"-webkit-transition: all 0.2s;\nbox-shadow: 4px 6px 12px 2px #888888;\n}\n"
		"button:hover {\nopacity: 0.8;\nbox-shadow: none;\n}\n"
		".error {\nbackground-color: #df3120;\npadding: 5px;\ntext-align: center;\n"
		"border: 3px solid #9f0000;\nfont-size: 22pt;\nborder-radius: 8px;\n"
		"margin: 10px;\n}</style>\n"
		"</head>\n"
		"<body><center><form action=\"/login\" method=\"post\">\n"
		"<!--ERROR-->\n"
		"<label for=\"name\">Username</label>\n"
		"<input name=\"name\" type=\"text\" id=\"name\" required placeholder=\"Username\"><br>\n"
		"<label for=\"name\">Password</label>\n"
		"<input name=\"pwd\" type=\"password\" id=\"pwd\" required placeholder=\"Password\"><br>\n"
		"<button type=\"submit\">Login</button>\n"
		"</form></center></body></html>";

const std::string ERROR_HTML = "<p class=\"error\">$error</p>";

const std::string DEFAULT_USERNAME = "admin";
const std::string DEFAULT_PASSWORD = "test";

const bool REQUIRE_LOGIN = true;

std::map<std::string, std::string> login_data;
std::map<std::string, std::string> sessions;
std::map<std::string, std::string> session_users;

uint32_t color = 0xffff00;
uint8_t rgb[3];

std::string get_session(AsyncWebServerRequest *request) {
	if (!request->hasParam("s")) {
		return "";
	}

	if (!request->hasHeader("Cookie")) {
		return "";
	}

	std::string cookiesStr = request->getHeader("Cookie")->value().c_str();

	std::map<std::string, std::string> cookies;
	size_t pos = 0;
	while ((pos = cookiesStr.find("; ")) != std::string::npos) {
		std::string cookie;
		cookie = cookiesStr.substr(0, pos);
		cookiesStr.erase(0, pos + 2);
		cookies[cookie.substr(0, cookie.find("="))] = cookie.substr(cookie.find("=") + 1);
	}
	cookies[cookiesStr.substr(0, cookiesStr.find("="))] = cookiesStr.substr(cookiesStr.find("=") + 1);

	std::string arg = request->getParam("s")->value().c_str();

	std::string cookie = cookies[arg.substr(0, 18).append("Key")];

	if (cookie == "") {
		return "";
	}

	if (cookie != sessions[arg]) {
		return "";
	}

	return arg;
}

std::pair<std::string, std::string> check_login(AsyncWebServerRequest *request) {
	std::string session = get_session(request);
	std::string user = session_users[session];

	if (session == "") {
		if (REQUIRE_LOGIN) {
			AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
			response->addHeader("Location", "/login");
			request->send(response);
		} else {
			user = "guest";
		}
	}

	std::pair<std::string, std::string> pair(user, session);
	return pair;
}

void on_not_found(AsyncWebServerRequest *request) {
	request->send(404, "text/plain", "Not found");
}

void on_get_index(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);
	if (REQUIRE_LOGIN && session.first == "") {
		return;
	}
	std::ostringstream os;
	os << '#';
	os.fill('0');
	os << std::setw(6);
	os << std::hex;
	os << std::right;
	os << color;
	std::string response = std::regex_replace(MAIN_PAGE, std::regex("\\$color"), os.str());
	response = std::regex_replace(response, std::regex("\\$user"), session.first);
	if (session.first == "guest") {
		response = std::regex_replace(response, std::regex("\\$logout\">Logout"), "/login\">Login");
	} else {
		response = std::regex_replace(response, std::regex("\\$logout"), std::string("/logout?s=").append(session.second));
	}
	request->send(200, "text/html", response.c_str());
}

void on_post_index(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);
	if (REQUIRE_LOGIN && session.first == "") {
		return;
	}
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

void on_get_login(AsyncWebServerRequest *request) {
	request->send(200, "text/html", LOGIN_PAGE.c_str());
}

void on_post_login(AsyncWebServerRequest *request) {
	std::string error = "Login Failed!";
	if (request->hasParam("name", true) && request->hasParam("pwd", true)) {
		std::string name = request->getParam("name", true)->value().c_str();
		std::string password = request->getParam("pwd", true)->value().c_str();
		if (login_data[name] != "") {
			if (login_data[name] == password) {
				AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
				std::string arg = uuid::generate_uuid();
				std::string cookie = uuid::generate_uuid();
				sessions[arg] = cookie;
				session_users[arg] = name;
				response->addHeader("Location", std::string("/index?s=").append(arg).c_str());
				response->addHeader("Set-Cookie", arg.substr(0, 18).append("Key=").append(cookie).append("; HttpOnly").c_str());
				request->send(response);
			} else {
				error = "Invalid password!";
			}
		} else {
			error = "Unknown username!";
		}
	} else {
		error = "Missing password or name!";
	}
	error = std::regex_replace(ERROR_HTML, std::regex("\\$error"), error);
	std::string response = std::regex_replace(LOGIN_PAGE, std::regex("<!--ERROR-->"), error);
	request->send(200, "text/html", response.c_str());
}

void on_get_logout(AsyncWebServerRequest *request) {
	std::string session = get_session(request);
	sessions[session] = "";
	session_users[session] = "";

	AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
	if (REQUIRE_LOGIN || session != "") {
		response->addHeader("Location", "/login");
	} else {
		response->addHeader("Location", "/");
	}
	request->send(response);
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
	server.on("/index", HTTP_GET, on_get_index);
	server.on("/index", HTTP_POST, on_post_index);

	server.on("/login", HTTP_GET, on_get_login);
	server.on("/login", HTTP_POST, on_post_login);

	server.on("/logout", HTTP_GET, on_get_logout);
	server.on("/logout", HTTP_POST, on_get_logout);

	server.onNotFound(on_not_found);

	server.begin();

	login_data[DEFAULT_USERNAME] = DEFAULT_PASSWORD;
}

void loop() {
	ledcWrite(CHANNEL_RED, rgb[0]);
	ledcWrite(CHANNEL_GREEN, rgb[1]);
	ledcWrite(CHANNEL_BLUE, rgb[2]);
	delay(100);
}

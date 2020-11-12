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

const std::string INDEX_HTML =
		"<!DOCTYPE html>\n"
		"<html>\n<head>\n"
		"<meta charset=\"utf-8\">\n"
		"<title>Esp32 Led color control</title>\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"/index.css\">\n"
		"</head>\n"
		"<body bgcolor=\"$color\">\n"
		"<div class=\"header\">\n"
		"<a class=\"headerpadding plainlink hoveropacity index\" href=\"$index\">Home</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity settings\" href=\"$settings\" hidden>Settings</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity sessions\" href=\"$sessions\" hidden>Sessions</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity logout\" href=\"$logout\">Logout</a>\n"
		"<a class=\"headerpadding hoveropacity user\">$user</a>\n"
		"</div>\n<center>\n"
		"<form method=\"post\">\n"
		"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\" "
		"required title=\"Submit hex value\" placeholder=\"$color\">\n<br/>\n"
		"<input type=\"submit\" value=\"Submit\">\n"
		"</form>\n"
		"</center>\n</body>\n</html>\n";

const std::string LOGIN_HTML =
		"<!DOCTYPE html>\n"
		"<html>\n<head>\n"
		"<meta charset=\"utf-8\">\n"
		"<title>Login</title>\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"/login.css\">\n"
		"</head>\n"
		"<body>\n<center>\n"
		"<form action=\"/index\" id=\"back\" hidden>\n"
		"</form>\n"
		"<form method=\"post\">\n"
		"<big>\n<b>ESP32 Led color control Login:</b>\n</big>\n<br/>\n"
		"<p class=\"error\" hidden>$error</p>\n"
		"<label for=\"name\">Username</label>\n"
		"<input name=\"name\" type=\"text\" id=\"name\" required placeholder=\"Username\">\n<br/>\n"
		"<label for=\"name\">Password</label>\n"
		"<input name=\"pwd\" type=\"password\" id=\"pwd\" required placeholder=\"Password\">\n<br/>\n"
		"<button form=\"back\" type=\"submit\" class=\"back\" $back>Back</button>\n"
		"<button type=\"submit\" class=\"login\">Login</button>\n"
		"</form>\n"
		"</center>\n</body>\n</html>\n";

const std::string SESSIONS_HTML =
		"<!DOCTYPE html>\n"
		"<html>\n<head>\n"
		"<meta charset=\"utf-8\">\n"
		"<title>Temporary Sessions</title>\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"/sessions.css\">\n"
		"</head>\n"
		"<body>\n"
		"<div class=\"header\">\n"
		"<a class=\"headerpadding plainlink hoveropacity index\" href=\"$index\">Home</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity settings\" href=\"$settings\" hidden>Settings</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity sessions\" href=\"$sessions\" hidden>Sessions</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity logout\" href=\"$logout\">Logout</a>\n"
		"<a class=\"headerpadding hoveropacity user\">$user</a>\n"
		"</div>\n<center>\n"
		"<div class=\"main\">\n"
		"<big>\n<b>Temporary Logins</b>\n</big>\n<br/>\n"
		"<small>All the temporary logins. They will be deleted with the next restart.</small>\n<br/>\n"
		"<!--SESSIONS-->\n"
		"<br/>\n"
		"<big>\n<b>Create temporary login</b>\n</big>\n<br/>\n"
		"<small>All logins created with this function will be deleted with the next restart.</small>\n"
		"<p class=\"error\" hidden>$error</p>\n"
		"<form method=\"post\">\n"
		"<b>\n<p>Login Data:</p>\n</b>\n"
		"<label for=\"name\">Username</label>\n"
		"<input id=\"name\" name=\"name\" placeholder=\"Username\" required type=\"text\">\n<br/>\n"
		"<label for=\"password\">Password</label>\n"
		"<input id=\"password\" name=\"password\" placeholder=\"Password\" required type=\"text\">\n<br/>\n"
		"<button class=\"create\" type=\"submit\">Create</button>\n"
		"</form>\n"
		"</div>\n"
		"</center>\n</body>\n</html>\n";

const std::string SESSIONS_SESSION_HTML =
		"<form method=\"post\">\n"
		"<div>\n"
		"<p>Username: $name</p>\n"
		"<p>Password: $password</p>\n"
		"</div>\n"
		"<button class=\"delete\" type=\"submit\" name=\"delete\" value=\"$name\">Delete</button>\n"
		"</form>\n"
		"<!--SESSIONS-->";

const std::string SETTINGS_HTML =
		"<!DOCTYPE html>\n"
		"<html>\n<head>\n"
		"<meta charset=\"utf-8\">\n"
		"<title>Settings</title>\n"
		"<link rel=\"stylesheet\" type=\"text/css\" href=\"/settings.css\">\n"
		"</head>\n"
		"<body>\n"
		"<div class=\"header\">\n"
		"<a class=\"headerpadding plainlink hoveropacity index\" href=\"$index\">Home</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity settings\" href=\"$settings\" hidden>Settings</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity sessions\" href=\"$sessions\" hidden>Sessions</a>\n"
		"<a class=\"headerpadding plainlink hoveropacity logout\" href=\"$logout\">Logout</a>\n"
		"<a class=\"headerpadding hoveropacity user\">$user</a>\n"
		"</div>\n<center>\n"
		"<form method=\"post\">\n"
		"<input type=\"checkbox\" id=\"require_login\" name=\"require_login\" $require_login>\n"
		"<label for=\"require_login\">Require Login</label>\n<br/>\n"
		"<button type=\"submit\">Submit</button>\n"
		"</form>\n"
		"</center>\n</body>\n</html>\n";

const std::string NOT_FOUND_HTML =
		"<!DOCTYPE html>\n"
		"<html>\n<head>\n"
		"<meta charset=\"utf-8\">\n"
		"<title>404 Not Found</title>\n"
		"</head>\n"
		"<body>\n"
		"<h1>Error 404 Not Found!</h1>\n"
		"<a href=\"/\">Main Page</a>\n"
		"<p>Accessed page: $url</p>\n"
		"</body>\n</html>\n";

const std::string INDEX_CSS =
		"body {\n  padding: 0px;\n  margin: 0px;\n}\n"
		"input {\n  background-color: #d3d3d319;\n  margin: 8px 0;\n  border-radius: 4px;\n"
		"  border: 1px solid #c0c0c0;\n  transition: background-color .2s;\n}\n"
		"input:hover {\n  background-color: #d3d3d388;\n}\n"
		".header {\n  background-color: #858585;\n  margin: 0px;\n  width: calc(100% - 8px);\n"
		"  border: 4px solid #737373;\n  text-align: right;\n  margin-bottom: 5px;\n}\n"
		".user {\n  background-color: #b5fdff;\n  cursor: pointer;\n}\n"
		".logout {\n  background-color: #ff5e3a;\n  cursor: pointer;\n}\n"
		".sessions {\n  background-color: #4954ff;\n  cursor: pointer;\n}\n"
		".settings {\n  background-color: #ff7725;\n  cursor: pointer;\n}\n"
		".index {\n  background-color: #49ff3a;\n  cursor: pointer;\n  float: left;\n}\n"
		".headerpadding {\n  padding: 15px;\n  margin: 0px;\n  display: inline-block;\n}\n"
		".hoveropacity {\n  opacity: 0.5;\n  transition: opacity 0.2s;\n"
		"  -webkit-transition: opacity 0.2s;\n}\n"
		".hoveropacity:hover {\n  opacity: 0.7;\n}\n"
		".plainlink {\n  text-decoration: none;\n}\n"
		".plainlink:link {\n  color: #000000;\n}\n"
		".plainlink:visited {\n  color: #000000;\n}\n"
		"[hidden] {\n  display: none !important;\n}\n";

const std::string LOGIN_CSS =
		"form {\n  background-color: #f0f0f0;\n  border: 4px solid #d3d3d3;\n"
		"  padding: 50px;\n  padding-top: 30px;\n  border-radius: 8px;\n  display: inline-block;\n}\n"
		"input {\n  padding: 12px 20px;\n  margin: 8px 0;\n  display: inline-block;\n"
		"  border: 1px solid #c0c0c0;\n  border-radius: 20px;\n  box-sizing: border-box;\n}\n"
		"button {\n  color: #ffffff;\n  margin: 8px 6px;\n  border: none;\n  border-radius: 12px;\n"
		"  cursor: pointer;\n  transition: all 0.2s;\n  -webkit-transition: all 0.2s;\n"
		"  box-shadow: 4px 6px 12px 2px #888888;\n}\n"
		"button:hover {\n  opacity: 0.8;\n  box-shadow: none;\n}\n"
		".login {\n  background-color: #4cc550;\n  padding: 14px 80px;\n}\n"
		".back {\n  background-color: #ff3333;\n  padding: 14px 20px;\n}\n"
		".error {\n  background-color: #df3120;\n  padding: 5px;\n  text-align: center;\n"
		"  border: 3px solid #9f0000;\n  font-size: 22pt;\n  border-radius: 8px;\n"
		"  margin: 10px;\n}\n"
		"[hidden] {\n  display: none !important;\n}\n";

const std::string SESSIONS_CSS =
		"body {\n  padding: 0px;\n  margin: 0px;\n}\n"
		"div {\n  display: inline-block;\n}\n"
		"form {\n  background-color: #f8f8f8;\n  border: 1px solid #000000;\n  padding: 5px;\n"
		"  border-radius: 4px;\n  margin: 3px;\n  text-align: left;\n  overflow: auto;\n}\n"
		"p {\n  margin: 0.2rem;\n}\n"
		"input[type=text] {\n  padding: 5px 20px;\n  margin: 2px;\n  display: inline-block;\n"
		"  border: 1px solid #c0c0c0;\n  border-radius: 20px;\n  box-sizing: border-box;\n}\n"
		".main {\n  background-color: #f0f0f0;\n  border: 4px solid #d3d3d3;\n"
		"  padding: 30px;\n  border-radius: 8px;\n}\n"
		".delete {\n  height: calc(2.8rem - 4px);\n  margin: 0.2rem;\n  margin-left: 20px;\n"
		"  background-color: #d53445;\n  border-radius: 6px;\n  float: right;\n"
		"  border: 2px solid #f04052;\n  transition: all 0.2s;\n  -webkit-transition: all 0.2s;\n}\n"
		".delete:hover {\n  background-color: #ff2210;\n  border-color: #d53545;\n}\n"
		".create {\n  background-color: #0088ff;\n  color: #ffffff;\n  padding: 14px 100px;\n"
		"  margin: 8px;\n  border: none;\n  border-radius: 12px;\n  cursor: pointer;\n"
		"  box-shadow: 4px 6px 12px 2px #888888;\n  transition: all 0.2s;\n"
		"  -webkit-transition: all 0.2s;\n}\n"
		".create:hover {\n  opacity: 0.9;\n  box-shadow: 2px 3px 6px 1px #888888;\n}\n"
		".create:active {\n  opacity: 0.8;\n  box-shadow: none;\n}\n"
		".error {\n  background-color: #df3120;\n  padding: 5px;\n  text-align: center;\n"
		"  border: 3px solid #9f0000;\n  font-size: 22pt;\n  border-radius: 8px;\n"
		"  margin: 10px;\n}\n"
		".header {\n  background-color: #858585;\n  margin: 0px;\n  width: calc(100% - 8px);\n"
		"  border: 4px solid #737373;\n  text-align: right;\n  margin-bottom: 5px;\n}\n"
		".user {\n  background-color: #b5fdff;\n  cursor: pointer;\n}\n"
		".logout {\n  background-color: #ff5e3a;\n  cursor: pointer;\n}\n"
		".sessions {\n  background-color: #4954ff;\n cursor: pointer;\n}\n"
		".settings {\n  background-color: #ff7725;\n  cursor: pointer;\n}\n"
		".index {\n  background-color: #49ff3a;\n  cursor: pointer;\n  float: left;\n}\n"
		".headerpadding {\n  padding: 15px;\n  margin: 0px;\n  display: inline-block;\n}\n"
		".hoveropacity {\n  opacity: 0.5;\n  transition: opacity 0.2s;\n"
		"  -webkit-transition: opacity 0.2s;\n}\n"
		".hoveropacity:hover {\n  opacity: 0.7;\n}\n"
		".plainlink {\n  text-decoration: none;\n}\n"
		".plainlink:link {\n  color: #000000;\n}\n"
		".plainlink:visited {\n  color: #000000;\n}\n"
		"[hidden] {\n  display: none !important;\n}\n";

const std::string SETTINGS_CSS =
		"body {\n  padding: 0px;\n  margin: 0px;\n}\n"
		"div {\n  display: inline-block;\n}\n"
		"form {\n  display: inline-block;\n  background-color: #f0f0f0;\n  border: 4px solid #d3d3d3;\n"
		"  padding: 20px;\n  border-radius: 8px;\n}\n"
		"button {\n  background-color: #0088ff;\n  color: #ffffff;\n  padding: 14px 100px;\n"
		"  margin: 8px;\n  border: none;\n  border-radius: 12px;\n  cursor: pointer;\n"
		"  box-shadow: 4px 6px 12px 2px #888888;\n  transition: all 0.2s;\n  -webkit-transition: all 0.2s;\n}\n"
		"button:hover {\n  box-shadow: 2px 3px 6px 1px #888888;\n  opacity: 0.9;\n}\n"
		"button:active {\n  opacity: 0.8;\n  box-shadow: none;\n}\n"
		".header {\n  background-color: #858585;\n  margin: 0px;\n  width: calc(100% - 8px);\n"
		"  border: 4px solid #737373;\n  text-align: right;\n  margin-bottom: 5px;\n}\n"
		".user {\n  background-color: #b5fdff;\n  cursor: pointer;\n}\n"
		".logout {\n  background-color: #ff5e3a;\n  cursor: pointer;\n}\n"
		".sessions {\n  background-color: #4954ff;\n cursor: pointer;\n}\n"
		".settings {\n  background-color: #ff7725;\n  cursor: pointer;\n}\n"
		".index {\n  background-color: #49ff3a;\n  cursor: pointer;\n  float: left;\n}\n"
		".headerpadding {\n  padding: 15px;\n  margin: 0px;\n  display: inline-block;\n}\n"
		".hoveropacity {\n  opacity: 0.5;\n  transition: opacity 0.2s;\n  -webkit-transition: opacity 0.2s;\n}\n"
		".hoveropacity:hover {\n  opacity: 0.7;\n}\n"
		".plainlink {\n  text-decoration: none;\n}\n"
		".plainlink:link {\n  color: #000000;\n}\n"
		".plainlink:visited {\n  color: #000000;\n}\n"
		"[hidden] {\n  display: none !important;\n}\n";

const std::string DEFAULT_USERNAME = "admin";
const std::string DEFAULT_PASSWORD = "test";

const std::string GUEST_USERNAME = "guest";

bool require_login = true;

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
		if (require_login) {
			AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
			response->addHeader("Location", "/login");
			request->send(response);
		} else {
			user = GUEST_USERNAME;
		}
	}

	std::pair<std::string, std::string> pair(user, session);
	return pair;
}

std::string finish_header(std::string page, std::string username, std::string session_id) {
	page = std::regex_replace(page, std::regex("\\$user"), username);

	if (username == GUEST_USERNAME) {
		page = std::regex_replace(page, std::regex("\\$logout\">Logout"), "/login\">Login");
		page = std::regex_replace(page, std::regex("\\$index"), "/index");
	} else {
		page = std::regex_replace(page, std::regex("\\$logout"), std::string("/logout?s=").append(session_id));
		page = std::regex_replace(page, std::regex("\\$index"), std::string("/index?s=").append(session_id));
	}

	if (username == DEFAULT_USERNAME) {
		std::string sessions = std::string("/sessions?s=").append(session_id).append("\"");
		page = std::regex_replace(page, std::regex("\\$sessions\" hidden"), sessions);

		std::string settings = std::string("/settings?s=").append(session_id).append("\"");
		page = std::regex_replace(page, std::regex("\\$settings\" hidden"), settings);
	}

	return page;
}

void on_get_index(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);
	if (require_login && session.first == "") {
		return;
	}

	std::ostringstream os;
	os << '#';
	os.fill('0');
	os << std::setw(6);
	os << std::hex;
	os << std::right;
	os << color;
	std::string response = finish_header(INDEX_HTML, session.first, session.second);

	response = std::regex_replace(response, std::regex("\\$color"), os.str());

	request->send(200, "text/html", response.c_str());
}

void on_post_index(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);
	if (require_login && session.first == "") {
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
	std::string response = std::regex_replace(LOGIN_HTML, std::regex("\\$back"), require_login ? "hidden" : "");
	request->send(200, "text/html", response.c_str());
}

void on_post_login(AsyncWebServerRequest *request) {
	std::string error = "Login Failed!";
	if (request->hasParam("name", true) && request->hasParam("pwd", true)) {
		std::string name = request->getParam("name", true)->value().c_str();
		std::string password = request->getParam("pwd", true)->value().c_str();
		if (login_data.find(name) != login_data.end()) {
			if (login_data[name] == password) {
				AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
				std::string arg = uuid::generate_uuid();
				std::string cookie = uuid::generate_uuid();
				sessions[arg] = cookie;
				session_users[arg] = name;
				response->addHeader("Location", std::string("/index?s=").append(arg).c_str());
				response->addHeader("Set-Cookie", arg.substr(0, 18).append("Key=").append(cookie).append("; HttpOnly").c_str());
				request->send(response);
				return;
			} else {
				error = "Invalid password!";
			}
		} else {
			error = "Unknown username!";
		}
	} else {
		error = "Missing password or name!";
	}
	std::string response = std::regex_replace(LOGIN_HTML, std::regex("hidden>\\$error"), std::string(">").append(error));
	request->send(200, "text/html", response.c_str());
}

void on_get_logout(AsyncWebServerRequest *request) {
	std::string session = get_session(request);
	sessions[session] = "";
	session_users[session] = "";

	AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
	if (require_login || session != "") {
		response->addHeader("Location", "/login");
	} else {
		response->addHeader("Location", "/");
	}
	request->send(response);
}

void on_get_sessions(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		std::string response = finish_header(SESSIONS_HTML, session.first, session.second);

		for (auto const& login : login_data) {
			if (login.first != DEFAULT_USERNAME) {
				std::string session_entry = SESSIONS_SESSION_HTML;
				session_entry = std::regex_replace(session_entry, std::regex("\\$name"), login.first);
				session_entry = std::regex_replace(session_entry, std::regex("\\$password"), login.second);
				response = std::regex_replace(response, std::regex("<!--SESSIONS-->"), session_entry);
			}
		}

		request->send(200, "text/html", response.c_str());
	} else {
		AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
		if (require_login) {
			if (session.first == "") {
				response->addHeader("Location", "/login");
			} else {
				response->addHeader("Location", std::string("/index?s=").append(session.second).c_str());
			}
		} else {
			response->addHeader("Location", "/");
		}
		request->send(response);
	}
}

void on_post_sessions(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		std::string error = "";
		if (request->hasParam("delete", true)) {
			std::string username = request->getParam("delete", true)->value().c_str();

			if (username == DEFAULT_USERNAME) {
				error = "Can't delete admin login data!";
			} else if (login_data.find(username) == login_data.end()) {
				error = "Username not found!";
			} else {
				login_data.erase(login_data.find(username));

				for (auto const& sess : session_users) {
					if (sess.second == username) {
						sessions.erase(sessions.find(sess.first));
						session_users.erase(session_users.find(sess.first));
					}
				}
			}
		} else if (request->hasParam("name", true) && request->hasParam("password", true)) {
			std::string username = request->getParam("name", true)->value().c_str();
			std::string password = request->getParam("password", true)->value().c_str();

			if (login_data.find(username) == login_data.end()) {
				login_data[username] = password;
			} else {
				error = "A user with that username already exists!";
			}
		}

		std::string response = finish_header(SESSIONS_HTML, session.first, session.second);

		for (auto const& login : login_data) {
			if (login.first != DEFAULT_USERNAME) {
				std::string session_entry = SESSIONS_SESSION_HTML;
				session_entry = std::regex_replace(session_entry, std::regex("\\$name"), login.first);
				session_entry = std::regex_replace(session_entry, std::regex("\\$password"), login.second);
				response = std::regex_replace(response, std::regex("<!--SESSIONS-->"), session_entry);
			}
		}

		if (error != "") {
			response = std::regex_replace(response, std::regex("hidden>\\$error"), std::string(">").append(error));
		}

		request->send(200, "text/html", response.c_str());
	} else {
		on_get_sessions(request);
	}
}

void on_get_settings(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		std::string response = finish_header(SETTINGS_HTML, session.first, session.second);;

		response = std::regex_replace(response, std::regex("\\$require_login"), require_login ? "checked" : "");

		request->send(200, "text/html", response.c_str());
	} else {
		AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
		if (require_login) {
			if (session.first == "") {
				response->addHeader("Location", "/login");
			} else {
				response->addHeader("Location", std::string("/index?s=").append(session.second).c_str());
			}
		} else {
			response->addHeader("Location", "/");
		}
		request->send(response);
	}
}

void on_post_settings(AsyncWebServerRequest *request) {
	std::pair<std::string, std::string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		// read all the settings values
		require_login = request->hasParam("require_login", true);
	}

	on_get_settings(request);
}

void on_not_found(AsyncWebServerRequest *request) {
	std::string response = std::regex_replace(NOT_FOUND_HTML, std::regex("\\$url"), request->url().c_str());
	request->send(404, "text/html", response.c_str());
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

	// html files
	server.on("/", HTTP_GET, on_get_index);
	server.on("/", HTTP_POST, on_post_index);
	server.on("/index", HTTP_GET, on_get_index);
	server.on("/index", HTTP_POST, on_post_index);

	server.on("/login", HTTP_GET, on_get_login);
	server.on("/login", HTTP_POST, on_post_login);

	server.on("/logout", HTTP_GET, on_get_logout);

	server.on("/sessions", HTTP_GET, on_get_sessions);
	server.on("/sessions", HTTP_POST, on_post_sessions);

	server.on("/settings", HTTP_GET, on_get_settings);
	server.on("/settings", HTTP_POST, on_post_settings);

	// stylesheets
	server.on("/index.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", INDEX_CSS.c_str());
	});
	server.on("/login.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", LOGIN_CSS.c_str());
	});
	server.on("/sessions.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", SESSIONS_CSS.c_str());
	});
	server.on("/settings.css", HTTP_GET, [] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", SETTINGS_CSS.c_str());
	});

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

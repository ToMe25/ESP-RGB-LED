/*
 * Webserverhandler.h
 *
 *  Created on: 12.11.2020
 *      Author: ToMe25
 */

#ifndef SRC_WEBSERVERHANDLER_H_
#define SRC_WEBSERVERHANDLER_H_

#include <string>
#include <map>
#include <ESPAsyncWebServer.h>

class WebserverHandler {
public:
	WebserverHandler();
	virtual ~WebserverHandler();

	void init();

	void on_get_index(AsyncWebServerRequest *request);
	void on_post_index(AsyncWebServerRequest *request);
	void on_get_login(AsyncWebServerRequest *request);
	void on_post_login(AsyncWebServerRequest *request);
	void on_get_logout(AsyncWebServerRequest *request);
	void on_get_sessions(AsyncWebServerRequest *request);
	void on_post_sessions(AsyncWebServerRequest *request);
	void on_get_settings(AsyncWebServerRequest *request);
	void on_post_settings(AsyncWebServerRequest *request);

	void on_not_found(AsyncWebServerRequest *request);

private:
	std::string get_session(AsyncWebServerRequest *request);
	std::pair<std::string, std::string> check_login(AsyncWebServerRequest *request);
	std::string finish_header(std::string page, std::string username, std::string session_id);

	AsyncWebServer server = AsyncWebServer(PORT);

	bool require_login = true;

	std::map<std::string, std::string> login_data;
	std::map<std::string, std::string> sessions;
	std::map<std::string, std::string> session_users;

	static const uint PORT = 80;

	const char *DEFAULT_USERNAME = "admin";
	const char *DEFAULT_PASSWORD = "test";
	const char *GUEST_USERNAME = "guest";

	const char *HEADER_HTML =
			"<div class=\"header\">\n"
			"<a class=\"headerelement index\" href=\"$index\">Home</a>\n"
			"<a class=\"headerelement settings\" href=\"$settings\" hidden>Settings</a>\n"
			"<a class=\"headerelement sessions\" href=\"$sessions\" hidden>Sessions</a>\n"
			"<a class=\"headerelement logout\" href=\"$logout\">Logout</a>\n"
			"<a class=\"headerelement user\">$user</a>\n"
			"</div>\n";

	const std::string INDEX_HTML =
			std::string(
					"<!DOCTYPE html>\n"
					"<html>\n<head>\n"
					"<meta charset=\"utf-8\">\n"
					"<title>Esp32 Led color control</title>\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/index.css\">\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/header.css\">\n"
					"</head>\n"
					"<body bgcolor=\"$color\">\n").append(HEADER_HTML).append(
							"<center>\n"
							"<form method=\"post\">\n"
							"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\" "
							"required title=\"Submit hex value\" placeholder=\"$color\">\n<br/>\n"
							"<input type=\"submit\" value=\"Submit\">\n"
							"</form>\n"
							"</center>\n</body>\n</html>\n");

	const char *LOGIN_HTML =
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
			std::string(
					"<!DOCTYPE html>\n"
					"<html>\n<head>\n"
					"<meta charset=\"utf-8\">\n"
					"<title>Temporary Sessions</title>\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/sessions.css\">\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/header.css\">\n"
					"</head>\n"
					"<body>\n").append(HEADER_HTML).append(
							"<center>\n"
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
							"</center>\n</body>\n</html>\n");

	const char *SESSIONS_SESSION_HTML =
			"<form method=\"post\">\n"
			"<div>\n"
			"<p>Username: $name</p>\n"
			"<p>Password: $password</p>\n"
			"</div>\n"
			"<button class=\"delete\" type=\"submit\" name=\"delete\" value=\"$name\">Delete</button>\n"
			"</form>\n"
			"<!--SESSIONS-->";

	const std::string SETTINGS_HTML =
			std::string(
					"<!DOCTYPE html>\n"
					"<html>\n<head>\n"
					"<meta charset=\"utf-8\">\n"
					"<title>Settings</title>\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/settings.css\">\n"
					"<link rel=\"stylesheet\" type=\"text/css\" href=\"/header.css\">\n"
					"</head>\n"
					"<body>\n").append(HEADER_HTML).append(
							"<center>\n"
							"<form method=\"post\">\n"
							"<input type=\"checkbox\" id=\"require_login\" name=\"require_login\" $require_login>\n"
							"<label for=\"require_login\">Require Login</label>\n<br/>\n"
							"<button type=\"submit\">Submit</button>\n"
							"</form>\n"
							"</center>\n</body>\n</html>\n");

	const char *NOT_FOUND_HTML = "<!DOCTYPE html>\n"
			"<html>\n<head>\n"
			"<meta charset=\"utf-8\">\n"
			"<title>404 Not Found</title>\n"
			"</head>\n"
			"<body>\n"
			"<h1>Error 404 Not Found!</h1>\n"
			"<a href=\"/\">Main Page</a>\n"
			"<p>Accessed page: $url</p>\n"
			"</body>\n</html>\n";

	const char *INDEX_CSS =
			"input {\n  background-color: #d3d3d319;\n  margin: 8px 0;\n  border-radius: 4px;\n"
			"  border: 1px solid #c0c0c0;\n  transition: background-color .2s;\n}\n"
			"input:hover {\n  background-color: #d3d3d388;\n}\n";

	const char *LOGIN_CSS =
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

	const char *SESSIONS_CSS =
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
			"  margin: 10px;\n}\n";

	const char *SETTINGS_CSS =
			"div {\n  display: inline-block;\n}\n"
			"form {\n  display: inline-block;\n  background-color: #f0f0f0;\n  border: 4px solid #d3d3d3;\n"
			"  padding: 20px;\n  border-radius: 8px;\n}\n"
			"button {\n  background-color: #0088ff;\n  color: #ffffff;\n  padding: 14px 100px;\n"
			"  margin: 8px;\n  border: none;\n  border-radius: 12px;\n  cursor: pointer;\n"
			"  box-shadow: 4px 6px 12px 2px #888888;\n  transition: all 0.2s;\n  -webkit-transition: all 0.2s;\n}\n"
			"button:hover {\n  box-shadow: 2px 3px 6px 1px #888888;\n  opacity: 0.9;\n}\n"
			"button:active {\n  opacity: 0.8;\n  box-shadow: none;\n}\n";

	const char *HEADER_CSS =
			"body {\n  padding: 0px;\n  margin: 0px;\n}\n"
			".header {\n  background-color: #858585;\n  margin: 0px;\n  width: calc(100% - 8px);\n"
			"  border: 4px solid #737373;\n  text-align: right;\n  margin-bottom: 5px;\n}\n"
			".user {\n  background-color: #b5fdff;\n  cursor: pointer;\n}\n"
			".logout {\n  background-color: #ff5e3a;\n  cursor: pointer;\n}\n"
			".sessions {\n  background-color: #4954ff;\n cursor: pointer;\n}\n"
			".settings {\n  background-color: #ff7725;\n  cursor: pointer;\n}\n"
			".index {\n  background-color: #49ff3a;\n  cursor: pointer;\n  float: left;\n}\n"
			".headerelement {\n  padding: 15px;\n  margin: 0px;\n  display: inline-block;\n"
			"  opacity: 0.5;\n  transition: opacity 0.2s;\n  -webkit-transition: opacity 0.2s;\n"
			"  text-decoration: none;\n}\n"
			".headerelement:hover {\n  opacity: 0.7;\n}\n"
			".headerelement:link {\n  color: #000000;\n}\n"
			".headerelement:visited {\n  color: #000000;\n}\n"
			"[hidden] {\n  display: none !important;\n}\n";
};

#endif

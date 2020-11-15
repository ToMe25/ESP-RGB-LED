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
#include <LedHandler.h>

class WebserverHandler {
public:
	WebserverHandler(LedHandler &led_handler, bool default_require_login = true);
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

	LedHandler &light;

	bool require_login;

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
					"<script type=\"text/javascript\" src=\"index.js\"></script>\n"
					"</head>\n"
					"<body style=\"background-color: $color;\">\n").append(HEADER_HTML).append(
							"<center>\n"
							"<form id=\"color_select\" name=\"color_select\" method=\"post\">\n"
							"<label for=\"color\">Select light color</label>\n<br/>\n"
							"<input name=\"color\" type=\"color\" value=\"$color\" id=\"color\" pattern=\"^#([A-Fa-f0-9]{6})$\""
							" required title=\"Submit hex value\" placeholder=\"#hexcolor\">\n<br/>\n"
							"Or select a color with these sliders\n<br/>\n"
							"<lable for=\"red\">Red:</lable>\n"
							"<input type=\"range\" value=\"127\" id=\"red\" pattern=\"^([0-9]{3})$\" required"
							" title=\"Submit red\" min=\"0\" max=\"255\" placeholder=\"100\" class=\"red\">\n<br/>\n"
							"<lable for=\"green\">Green:</lable>\n"
							"<input type=\"range\" value=\"127\" id=\"green\" pattern=\"^([0-9]{3})$\" required"
							" title=\"Submit green\" min=\"0\" max=\"255\" placeholder=\"100\" class=\"green\">\n<br/>\n"
							"<lable for=\"blue\">Blue:</lable>\n"
							"<input type=\"range\" value=\"127\" id=\"blue\" pattern=\"^([0-9]{3})$\" required"
							" title=\"Submit blue\" min=\"0\" max=\"255\" placeholder=\"100\" class=\"blue\">\n<br/>\n"
							"<label for=\"submit\">Apply the light color</label>\n"
							"<br/>\n"
							"<button id=\"submit\" type=\"submit\" value=\"Submit\" style=\"color: $color;\">Apply</button>\n"
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
			"<form method=\"post\">\n"
			"<big>\n<b>ESP32 Led color control Login:</b>\n</big>\n<br/>\n"
			"<p class=\"error\" hidden>$error</p>\n"
			"<label for=\"name\">Username</label>\n"
			"<input name=\"name\" type=\"text\" id=\"name\" required placeholder=\"Username\">\n<br/>\n"
			"<label for=\"name\">Password</label>\n"
			"<input name=\"pwd\" type=\"password\" id=\"pwd\" required placeholder=\"Password\">\n<br/>\n"
			"<button type=\"submit\" class=\"login\">Login</button>\n"
			"<button formaction=\"/index.html\" formnovalidate type=\"submit\" class=\"back\" $back>Back</button>\n"
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
			"form {\n  background-color: #d3d3d388;\n  display: inline-block;\n  padding: 10px;\n  border-radius: 6px;\n"
			"  border: 3px solid #d8d8d888;\n}\n"
			"input[type=color] {\n  background-color: #d3d3d319;\n  margin: 8px;\n  border-radius: 4px;\n"
			"  border: 1px solid #c0c0c0;\n  transition: background-color 0.2s;\n  -webkit-transition: background-color 0.2s;\n}\n"
			"input[type=color]:focus {\n  outline: none;\n}\n"
			"input[type=color]:hover {\n  background-color: #d3d3d399;\n}\n"
			"button {\n  margin: 8px;\n  padding: 8px 24px;\n  border-radius: 4px;\n  border: 3px solid #acacac;\n"
			"  transition: opacity 0.2s;\n  -webkit-transition: opacity 0.2s;\n}\n"
			"button:hover {\n  opacity: 0.85;\n}\n"
			"button:active {\n  opacity: 0.7;\n}\n"
			"input[type=range] {\n  background-color: #00000000;\n  border: none;\n  cursor: pointer;\n"
			"  -webkit-appearance: none;\n  transition: opacity 0.2s;\n  -webkit-transition: opacity 0.2s;\n}\n"
			"input[type=range]:focus {\n  outline: none;\n}\n"
			"input[type=range]:hover {\n  opacity: 0.8;\n}\n"
			"input[type=range]::-webkit-slider-runnable-track {\n  border-radius: 5px;\n  height: 1rem;\n}\n"
			"input[type=range]::-webkit-slider-thumb {\n  width: 18px;\n  height: 0.75rem;\n  border-radius: 35px;\n"
			"  -webkit-appearance: none;\n}\n"
			"input[type=range]::-moz-range-track {\n  border-radius: 5px;\n  height: 1rem;\n}\n"
			"input[type=range]::-moz-range-thumb {\n  width: 18px;\n  height: 0.75rem;\n  border-radius: 35px;\n}\n"
			"input[type=range].red::-webkit-slider-runnable-track {\n  background-color: #bf3232;\n""  border: 2px solid #a00000;\n}\n"
			"input[type=range].red::-webkit-slider-thumb {\n  background-color: #ff0000;\n  border: 2px solid #7f0000;\n}\n"
			"input[type=range].red::-moz-range-track {\n  background-color: #bf3232;\n  border: 2px solid #a00000;\n}\n"
			"input[type=range].red::-moz-range-thumb {\n  background-color: #ff0000;\n  border: 2px solid #7f0000;\n}\n"
			"input[type=range].green::-webkit-slider-runnable-track {\n  background-color: #32bf32;\n  border: 2px solid #00a000;\n}\n"
			"input[type=range].green::-webkit-slider-thumb {\n  background-color: #00ff00;\n  border: 2px solid #007f00;\n}\n"
			"input[type=range].green::-moz-range-track {\n  background-color: #32bf32;\n  border: 2px solid #00a000;\n}\n"
			"input[type=range].green::-moz-range-thumb {\n  background-color: #00ff00;\n  border: 2px solid #007f00;\n}\n"
			"input[type=range].blue::-webkit-slider-runnable-track {\n  background-color: #3232bf;\n  border: 2px solid #0000a0;\n}\n"
			"input[type=range].blue::-webkit-slider-thumb {\n  background-color: #0000ff;\n  border: 2px solid #00007f;\n}\n"
			"input[type=range].blue::-moz-range-track {\n  background-color: #3232bf;\n  border: 2px solid #0000a0;\n}\n"
			"input[type=range].blue::-moz-range-thumb {\n  background-color: #0000ff;\n  border: 2px solid #00007f;\n}\n"
;

	const char *LOGIN_CSS =
			"form {\n  background-color: #f0f0f0;\n  border: 4px solid #d3d3d3;\n"
			"  padding: 50px;\n  padding-top: 30px;\n  border-radius: 8px;\n  display: inline-block;\n}\n"
			"input {\n  padding: 12px 20px;\n  margin: 8px 0;\n  display: inline-block;\n"
			"  border: 1px solid #c0c0c0;\n  border-radius: 20px;\n  box-sizing: border-box;\n}\n"
			"button {\n  color: #ffffff;\n  margin: 8px 6px;\n  border: none;\n  border-radius: 12px;\n"
			"  cursor: pointer;\n  transition: all 0.2s;\n  -webkit-transition: all 0.2s;\n"
			"  box-shadow: 4px 6px 12px 2px #888888;\n}\n"
			"button:hover {\n  opacity: 0.8;\n  box-shadow: none;\n}\n"
			".login {\n  background-color: #4cc550;\n  padding: 14px 80px;\n  float: right;\n}\n"
			".back {\n  background-color: #ff3333;\n  padding: 14px 20px;\n  float: left;\n}\n"
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

	const char *INDEX_JS =
			"var colorSelect\nvar redSelect\nvar greenSelect\nvar blueSelect\nvar applyButton\nvar color\nvar interval\n\n"
			"window.onload = init\n\ninterval = window.setInterval(checkColor, 5000)\n\nfunction init() {\n"
			"  colorSelect = document.color_select.color\n  redSelect = document.color_select.red\n"
			"  greenSelect = document.color_select.green\n  blueSelect = document.color_select.blue\n"
			"  applyButton = document.getElementById('submit')\n\n"
			"  colorSelect.addEventListener(\"change\", onColorChange, false)\n"
			"  redSelect.addEventListener(\"change\", onColorChange, false)\n"
			"  greenSelect.addEventListener(\"change\", onColorChange, false)\n"
			"  blueSelect.addEventListener(\"change\", onColorChange, false)\n\n"
			"  color = parseInt(colorSelect.value.substring(1, 7), 16)\n  setColor(color)\n}\n\nfunction onColorChange() {\n"
			"  window.clearInterval(interval)\n  if (this.id == \"color\") {\n"
			"    color = parseInt(this.value.substring(1, 7), 16)\n  } else if (this.id == \"red\") {\n"
			"    color = color & 0x00FFFF | this.value << 16\n  } else if (this.id == \"green\") {\n"
			"    color = color & 0xFF00FF | this.value << 8\n  } else if (this.id == \"blue\") {\n"
			"    color = color & 0xFFFF00 | this.value\n  }\n  setColor(color)\n}\n\nfunction setColor(color) {\n"
			"  var hexColor = '#' + color.toString(16).padStart(6, '0')\n  document.body.style.backgroundColor = hexColor\n"
			"  colorSelect.value = hexColor\n  redSelect.value = color >> 16\n  greenSelect.value = (color & 0x00FF00) >> 8\n"
			"  blueSelect.value = color & 0x0000FF\n  applyButton.style.color = hexColor\n"
			"  applyButton.style.backgroundColor = '#' + (color ^ 0xFFFFFF).toString(16).padStart(6, '0')\n}\n\n"
			"function checkColor() {\n  fetch('color.json', {method: 'get'})\n  .then((res) => {\n    return res.json()\n"
			"  })\n  .then((out) => {\n    color = out.color\n    setColor(color)\n  })\n  .catch((err) => {\n"
			"    throw err\n  })\n}\n";
};

#endif

/*
 * Webserverhandler.cpp
 *
 *  Created on: 12.11.2020
 *      Author: ToMe25
 */

#include <WebserverHandler.h>
#include <regex>
#include <iomanip>
#include <uuid.h>
#include <main.h>

using namespace std;

WebserverHandler::WebserverHandler() {
	login_data[DEFAULT_USERNAME] = DEFAULT_PASSWORD;
}

WebserverHandler::~WebserverHandler() {
	WebserverHandler();
}

void WebserverHandler::init() {

	// html files
	server.on("/", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_index(request);});
	server.on("/", HTTP_POST, [this] (AsyncWebServerRequest *request) {on_post_index(request);});
	server.on("/index", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_index(request);});
	server.on("/index", HTTP_POST, [this] (AsyncWebServerRequest *request) {on_post_index(request);});

	server.on("/login", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_login(request);});
	server.on("/login", HTTP_POST, [this] (AsyncWebServerRequest *request) {on_post_login(request);});

	server.on("/logout", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_logout(request);});

	server.on("/sessions", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_sessions(request);});
	server.on("/sessions", HTTP_POST, [this] (AsyncWebServerRequest *request) {on_post_sessions(request);});

	server.on("/settings", HTTP_GET, [this] (AsyncWebServerRequest *request) {on_get_settings(request);});
	server.on("/settings", HTTP_POST, [this] (AsyncWebServerRequest *request) {on_post_settings(request);});

	// stylesheets
	server.on("/header.css", HTTP_GET, [this] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", HEADER_CSS);
	});
	server.on("/index.css", HTTP_GET, [this] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", INDEX_CSS);
	});
	server.on("/login.css", HTTP_GET, [this] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", LOGIN_CSS);
	});
	server.on("/sessions.css", HTTP_GET, [this] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", SESSIONS_CSS);
	});
	server.on("/settings.css", HTTP_GET, [this] (AsyncWebServerRequest *request) {
		request->send(200, "text/css", SETTINGS_CSS);
	});

	server.onNotFound([this] (AsyncWebServerRequest *request) {on_not_found(request);});

	server.begin();
}

string WebserverHandler::get_session(AsyncWebServerRequest *request) {
	if (!request->hasParam("s")) {
		return "";
	}

	if (!request->hasHeader("Cookie")) {
		return "";
	}

	string cookiesStr = request->getHeader("Cookie")->value().c_str();

	// no idea why this std:: is necessary.
	std::map<string, string> cookies;
	size_t pos = 0;
	while ((pos = cookiesStr.find("; ")) != string::npos) {
		string cookie;
		cookie = cookiesStr.substr(0, pos);
		cookiesStr.erase(0, pos + 2);
		cookies[cookie.substr(0, cookie.find("="))] = cookie.substr(cookie.find("=") + 1);
	}
	cookies[cookiesStr.substr(0, cookiesStr.find("="))] = cookiesStr.substr(cookiesStr.find("=") + 1);

	string arg = request->getParam("s")->value().c_str();

	string cookie = cookies[arg.substr(0, 18).append("Key")];

	if (cookie == "") {
		return "";
	}

	if (cookie != sessions[arg]) {
		return "";
	}

	return arg;
}

pair<string, string> WebserverHandler::check_login(AsyncWebServerRequest *request) {
	string session = get_session(request);
	string user = session_users[session];

	if (session == "") {
		if (require_login) {
			AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
			response->addHeader("Location", "/login");
			request->send(response);
		} else {
			user = WebserverHandler::GUEST_USERNAME;
		}
	}

	pair<string, string> pair(user, session);
	return pair;
}

string WebserverHandler::finish_header(string page, string username, string session_id) {
	page = regex_replace(page, regex("\\$user"), username);

	if (username == WebserverHandler::GUEST_USERNAME) {
		page = regex_replace(page, regex("\\$logout\">Logout"), "/login\">Login");
		page = regex_replace(page, regex("\\$index"), "/index");
	} else {
		page = regex_replace(page, regex("\\$logout"), string("/logout?s=").append(session_id));
		page = regex_replace(page, regex("\\$index"), string("/index?s=").append(session_id));
	}

	if (username == WebserverHandler::DEFAULT_USERNAME) {
		string sessions = string("/sessions?s=").append(session_id).append("\"");
		page = regex_replace(page, regex("\\$sessions\" hidden"), sessions);

		string settings = string("/settings?s=").append(session_id).append("\"");
		page = regex_replace(page, regex("\\$settings\" hidden"), settings);
	}

	return page;
}

void WebserverHandler::on_get_index(AsyncWebServerRequest *request) {
	pair<string, std::string> session = check_login(request);
	if (require_login && session.first == "") {
		return;
	}

	std::ostringstream os;
	os << '#';
	os.fill('0');
	os << setw(6);
	os << hex;
	os << right;
	os << main::getColor();
	std::string response = finish_header(INDEX_HTML, session.first, session.second);

	response = std::regex_replace(response, std::regex("\\$color"), os.str());

	request->send(200, "text/html", response.c_str());
}

void WebserverHandler::on_post_index(AsyncWebServerRequest *request) {
	pair<string, string> session = check_login(request);
	if (require_login && session.first == "") {
		return;
	}
	if (request->hasParam("color"), true) {
		uint color;
		std::istringstream is(
				request->getParam("color", true)->value().substring(1).c_str());
		is >> hex >> color;
		main::setColor(color);
	} else {
		Serial.println("Received a post request missing the color value.");
	}
	on_get_index(request);
}

void WebserverHandler::on_get_login(AsyncWebServerRequest *request) {
	string response = regex_replace(LOGIN_HTML, regex("\\$back"), require_login ? "hidden" : "");
	request->send(200, "text/html", response.c_str());
}

void WebserverHandler::on_post_login(AsyncWebServerRequest *request) {
	string error = "Login Failed!";
	if (request->hasParam("name", true) && request->hasParam("pwd", true)) {
		string name = request->getParam("name", true)->value().c_str();
		string password = request->getParam("pwd", true)->value().c_str();
		if (login_data.find(name) != login_data.end()) {
			if (login_data[name] == password) {
				AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
				string arg = uuid::generate_uuid();
				string cookie = uuid::generate_uuid();
				sessions[arg] = cookie;
				session_users[arg] = name;
				response->addHeader("Location", string("/index?s=").append(arg).c_str());
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
	string response = regex_replace(LOGIN_HTML, regex("hidden>\\$error"), string(">").append(error));
	request->send(200, "text/html", response.c_str());
}

void WebserverHandler::on_get_logout(AsyncWebServerRequest *request) {
	string session = get_session(request);
	sessions[session] = "";
	session_users[session] = "";

	AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
	if (require_login || session != "") {
		response->addHeader("Location", "/login");
	} else {
		response->addHeader("Location", "/");
	}
	response->addHeader("Set-Cookie", session.substr(0, 18).append("Key=").append("").append("; HttpOnly").c_str());
	request->send(response);
}

void WebserverHandler::on_get_sessions(AsyncWebServerRequest *request) {
	pair<string, string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		string response = finish_header(SESSIONS_HTML, session.first, session.second);

		for (auto const& login : login_data) {
			if (login.first != DEFAULT_USERNAME) {
				string session_entry = SESSIONS_SESSION_HTML;
				session_entry = regex_replace(session_entry, regex("\\$name"), login.first);
				session_entry = regex_replace(session_entry, regex("\\$password"), login.second);
				response = regex_replace(response, regex("<!--SESSIONS-->"), session_entry);
			}
		}

		request->send(200, "text/html", response.c_str());
	} else {
		AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
		if (require_login) {
			if (session.first == "") {
				response->addHeader("Location", "/login");
			} else {
				response->addHeader("Location", string("/index?s=").append(session.second).c_str());
			}
		} else {
			response->addHeader("Location", "/");
		}
		request->send(response);
	}
}

void WebserverHandler::on_post_sessions(AsyncWebServerRequest *request) {
	pair<string, string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		string error = "";
		if (request->hasParam("delete", true)) {
			string username = request->getParam("delete", true)->value().c_str();

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
			string username = request->getParam("name", true)->value().c_str();
			string password = request->getParam("password", true)->value().c_str();

			if (login_data.find(username) == login_data.end()) {
				login_data[username] = password;
			} else {
				error = "A user with that username already exists!";
			}
		}

		string response = finish_header(SESSIONS_HTML, session.first, session.second);

		for (auto const& login : login_data) {
			if (login.first != DEFAULT_USERNAME) {
				string session_entry = SESSIONS_SESSION_HTML;
				session_entry = regex_replace(session_entry, regex("\\$name"), login.first);
				session_entry = regex_replace(session_entry, regex("\\$password"), login.second);
				response = regex_replace(response, regex("<!--SESSIONS-->"), session_entry);
			}
		}

		if (error != "") {
			response = regex_replace(response, regex("hidden>\\$error"), string(">").append(error));
		}

		request->send(200, "text/html", response.c_str());
	} else {
		on_get_sessions(request);
	}
}

void WebserverHandler::on_get_settings(AsyncWebServerRequest *request) {
	pair<string, string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		string response = finish_header(SETTINGS_HTML, session.first, session.second);;

		response = regex_replace(response, regex("\\$require_login"), require_login ? "checked" : "");

		request->send(200, "text/html", response.c_str());
	} else {
		AsyncWebServerResponse *response = request->beginResponse(303, "text/plain", "See Other");
		if (require_login) {
			if (session.first == "") {
				response->addHeader("Location", "/login");
			} else {
				response->addHeader("Location", string("/index?s=").append(session.second).c_str());
			}
		} else {
			response->addHeader("Location", "/");
		}
		request->send(response);
	}
}

void WebserverHandler::on_post_settings(AsyncWebServerRequest *request) {
	pair<string, string> session = check_login(request);

	if (require_login && session.first == "") {
		return;
	}

	if (session.first == DEFAULT_USERNAME) {
		// read all the settings values
		require_login = request->hasParam("require_login", true);
	}

	on_get_settings(request);
}

void WebserverHandler::on_not_found(AsyncWebServerRequest *request) {
	string response = regex_replace(NOT_FOUND_HTML, regex("\\$url"), request->url().c_str());
	request->send(404, "text/html", response.c_str());
}

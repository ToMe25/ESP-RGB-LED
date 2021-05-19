/*
 * Webserverhandler.h
 *
 * Created on: 12.11.2020
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

#ifndef LIB_WEBSERVERHANDLER_WEBSERVERHANDLER_H_
#define LIB_WEBSERVERHANDLER_WEBSERVERHANDLER_H_

#include <string>
#include <map>
#include <ESPAsyncWebServer.h>
#include <LedHandler.h>

extern const char INDEX_HTML[] asm("_binary_src_html_index_html_start");
extern const char LOGIN_HTML[] asm("_binary_src_html_login_html_start");
extern const char NOT_FOUND_HTML[] asm("_binary_src_html_not_found_html_start");
extern const char SESSIONS_SESSION_HTML[] asm("_binary_src_html_sessions_session_html_start");
extern const char SESSIONS_HTML[] asm("_binary_src_html_sessions_html_start");
extern const char SETTINGS_HTML[] asm("_binary_src_html_settings_html_start");
extern const char HEADER_CSS[] asm("_binary_src_html_css_header_css_start");
extern const char INDEX_CSS[] asm("_binary_src_html_css_index_css_start");
extern const char MAIN_CSS[] asm("_binary_src_html_css_main_css_start");
extern const char INDEX_JS[] asm("_binary_src_html_index_js_start");
extern const char PROPERTIES_JSON[] asm("_binary_src_html_properties_json_start");
// includes the content of the file "adminpass.txt" in the project root.
// Make sure this file doesn't end with an empty line.
extern const char ADMIN_PASSWORD[] asm("_binary_adminpass_txt_start");

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

	void on_get_properties_json(AsyncWebServerRequest *request);

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

	const char *GUEST_USERNAME = "guest";
};

#endif

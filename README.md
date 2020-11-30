# ESP-RGB-LED
ESP-RGB-LED is ESP32 software to controll an attached RGB Led from a web interface.<br/>
The admin user can set whether you need to be logged in to controll the led color.<br/>
The admin user can also create temporary logins that can be used in the login screen.<br/>
These will be deleted next time the esp restarts(gets disconnected from power).<br>
The Require Login setting will also get reset when the esp restarts.

## Website
A list of the pages of the web interface.

The main color selection screen in single color mode:<br/>
![Esp32_Led_color_control](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Esp32_Led_color_control.png)
The color selection screen in dual color mode:<br/>
![Esp32_Led_dual_color_control](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Esp32_Led_dual_color_control.png)
The color selection screen as a normal user(single color):<br/>
![Esp32_Led_temp_session_color_control](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Esp32_Led_temp_session_color_control.png)
The login page(with login not required):<br>
![Login](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Login.png)
The admin settings page(can disable the login requirement):<br/>
![Settings](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Settings.png)
The screen for the creation and deletion of logins:<br>
![Temporary_Sessions](https://raw.githubusercontent.com/ToMe25/ESP-RGB-LED/master/images/Temporary_Sessions.png)

## Installation
1. Clone this repository. For example with `git clone https://www.github.com/ToMe25/ESP-RGB-LED.git/`.
2. Install [PlatformIO](https://docs.platformio.org/en/latest/core/installation.html).
3. Attach your ESP32 to the PC.
4. Replace `WIFI_SSID` and `WPA_PASSPHRASE` in [src/main.h](https://github.com/ToMe25/ESP-RGB-LED/blob/master/src/main.h) with your WLAN credentials.
5. Replace `ADMIN_PASSWORD` in [lib/WebserverHandler/WebserverHandler.h](https://github.com/ToMe25/ESP-RGB-LED/blob/master/lib/WebserverHandler/WebserverHandler.h) with the password for the default/administation user.
6. Build and upload ESP-RGB-LED to the ESP32 using `pio run --target upload`

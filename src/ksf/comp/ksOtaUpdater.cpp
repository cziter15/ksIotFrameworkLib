/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"
#include "ksOtaUpdater.h"

#if USE_ELEGANT_OTA
#if defined(ESP8266)
    #define HARDWARE "ESP8266"
    #include "ESP8266WiFi.h"
    #include "WiFiClient.h"
    #include "ESP8266WebServer.h"
    #include "ESP8266HTTPUpdateServer.h"

	ESP8266WebServer server(80);
	ESP8266HTTPUpdateServer httpUpdater;
#elif defined(ESP32)
    #define HARDWARE "ESP32"
    #include "WiFi.h"
    #include "WiFiClient.h"
    #include "WebServer.h"
    #include "Update.h"

	WebServer server(80);
#endif
#include "../res/otaWebpage.h"
#endif
namespace ksf::comps
{
	ksOtaUpdater::ksOtaUpdater()
		: ksOtaUpdater(PGM_("ota_ksiotframework"))
	{}

	ksOtaUpdater::ksOtaUpdater(const std::string& password)
	{
		ArduinoOTA.setPassword(password.c_str());

#if USE_ELEGANT_OTA
		webOtaPassword = password;
#endif

		ArduinoOTA.onStart([this]() {
			onUpdateStart->broadcast();
		});

		ArduinoOTA.onEnd([this]() {
			this->updateFinished();
		});
	}

	void ksOtaUpdater::updateFinished()
	{
		ksf::saveOtaBootIndicator();
		onUpdateEnd->broadcast();
	}
#if USE_ELEGANT_OTA
	void ksOtaUpdater::setupUpdateWebServer()
	{
		static const char* username = "admin";
		const char* password = webOtaPassword.c_str();

		server.on("/", HTTP_GET, [&]() {
			server.sendHeader("Location", "/update", true);
			server.send(302, "text/plain", "");
		});

		server.on("/update", HTTP_GET, [&]() {
			if (!server.authenticate(username, password))
				return server.requestAuthentication();

			server.sendHeader("Content-Encoding", "gzip");
			server.send_P(200, "text/html", (const char*)RES_OTA_WEBPAGE_HTML, RES_OTA_WEBPAGE_HTML_SIZE);
		});

		server.on("/update/identity", HTTP_GET, [&]() {
			if (!server.authenticate(username, password))
				return server.requestAuthentication();

			std::string json;
			json.reserve(64);
			json += "{ \"id\": \"";
			json += WiFi.getHostname();
			json += "\", \"hardware\": \"" HARDWARE "\" }";
			server.send(200, "application/json", json.c_str());
		});

#if defined(ESP8266)
		httpUpdater.setup(&server, "/update", username, password);
#elif defined(ESP32)
		server.on("/update", HTTP_POST, [&]() {
			if (authenticate && !server.authenticate(username, password))
				return;

			server.sendHeader("Connection", "close");
			server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
			delay(100);
			yield();
			delay(100);
			ESP.restart();
		}, [&]() {
			if (authenticate && !server.authenticate(username, password))
				return;

			HTTPUpload& upload = server.upload();
			if (upload.status == UPLOAD_FILE_START)
			{
				if (upload.name == "filesystem")
					Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
				else
					Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);
			}
			else if (upload.status == UPLOAD_FILE_WRITE)
			{
				Update.write(upload.buf, upload.currentSize);
			}
			else if (upload.status == UPLOAD_FILE_END) 
			{
				Update.end(true)
			}
		});
#endif
		server.begin();
	}
#endif
	
	bool ksOtaUpdater::init(ksApplication* owner)
	{
		ArduinoOTA.setHostname(WiFi.getHostname());
		ArduinoOTA.begin();

#if USE_ELEGANT_OTA
		setupUpdateWebServer();
#endif

		return true;
	}

	bool ksOtaUpdater::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

#if USE_ELEGANT_OTA
		/* Handle HTTP stuff. */
		server.handleClient();
#endif

		return true;
	}
}
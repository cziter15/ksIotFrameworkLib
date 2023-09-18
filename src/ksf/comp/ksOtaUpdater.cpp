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

	void ksOtaUpdater::setupUpdateWebServer()
	{
		const char* username = "admin";
		const char* password = webOtaPassword.c_str();

		server.on("/update", HTTP_GET, [&]() {
			if (!server.authenticate(username, password)) 
				return server.requestAuthentication();

			server.sendHeader("Content-Encoding", "gzip");
			server.send_P(200, "text/html", (const char*)RES_OTA_WEBPAGE_HTML, RES_OTA_WEBPAGE_HTML_SIZE);
		});
		
		webOtaIdentity = "{ \"id\": \"" + std::string(WiFi.getHostname()) + "\", \"hardware\": \"" HARDWARE "\" }";

		server.on("/update/identity", HTTP_GET, [&]() {
			if (!server.authenticate(username, password))
				return server.requestAuthentication();
			server.send(200, "application/json", WiFi.getHostname());
		});

#if defined(ESP8266)
			httpUpdater.setup(&server, "/update", username, password);
#elif defined(ESP32)
		_server->on("/update", HTTP_POST, [&]() {
			if (authenticate && !_server->authenticate(username, password))
				return;

			_server->sendHeader("Connection", "close");
			_server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
			#if defined(ESP32)
			// Needs some time for Core 0 to send response
			delay(100);
			yield();
			delay(100);
			#endif
			ESP.restart();
		}, [&]() {
			// Actual OTA Download
			if (authenticate && !_server->authenticate(username, password)) {
			return;
			}

			HTTPUpload& upload = _server->upload();
			if (upload.status == UPLOAD_FILE_START)
			{
				if (upload.name != "filesystem")
					Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH));
			} 
			else if (upload.status == UPLOAD_FILE_WRITE)
			{
			Update.write(upload.buf, upload.currentSize);
			} else if (upload.status == UPLOAD_FILE_END) {
			Update.end(true)
		});
#endif
	}
	
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
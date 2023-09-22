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
#include "ksDevicePortal.h"
#include "ksConfigProvider.h"

#if SUPPORT_HTTP_OTA
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
	ksDevicePortal::ksDevicePortal()
		: ksDevicePortal(PGM_("ota_ksiotframework"))
	{}

	ksDevicePortal::ksDevicePortal(const std::string& password)
	{
		ArduinoOTA.setPassword(password.c_str());

#if SUPPORT_HTTP_OTA
		this->password = password;
#endif

		ArduinoOTA.onStart([this]() {
			onUpdateStart->broadcast();
		});

		ArduinoOTA.onEnd([this]() {
			updateFinished();
		});
	}
	
	bool ksDevicePortal::init(ksApplication* owner)
	{
		this->owner = owner;
		ArduinoOTA.setHostname(WiFi.getHostname());
		ArduinoOTA.begin();

#if SUPPORT_HTTP_OTA
		setupUpdateWebServer();
#endif

		return true;
	}

	void ksDevicePortal::updateFinished()
	{
		ksf::saveOtaBootIndicator();
		onUpdateEnd->broadcast();
	}
#if SUPPORT_HTTP_OTA
	void ksDevicePortal::setupUpdateWebServer()
	{
		static const char* username{"admin"};

		server.onNotFound([&]() {
			server.sendHeader("Location", "/", true);
			server.send(302, "text/plain", "");
		});

		server.on("/", HTTP_GET, [&]() {
			if (!server.authenticate(username, password.c_str()))
				return server.requestAuthentication();

			server.sendHeader("Content-Encoding", "gzip");
			server.send_P(200, "text/html", (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
		});

		server.on("/api/identity", HTTP_GET, [&]() {
			if (!server.authenticate(username, password.c_str()))
				return server.requestAuthentication();

			std::string json;
			json.reserve(64);
			json += "{ \"id\": \"";
			json += WiFi.getHostname();
			json += "\", \"hardware\": \"" HARDWARE "\" }";
			server.send(200, "application/json", json.c_str());
		});

		server.on("/api/getDeviceParams", HTTP_GET, [&]() {
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);

			std::string response;
			response.reserve(64);
			response += "[";
			
			for (auto& configCompWp : configCompsWp) 
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters()) 
				{
					response += "{";
					response += "\"id\":\"" + parameter.id + "\",";
					response += "\"value\":\"" + parameter.defaultValue + "\",";
					response += "\"maxLen\":\"" + std::to_string(parameter.maxLength) + "\"";
					response += "},";
				}
			}

			if (response.back() == ',')
				response.pop_back();

			response += "]";

			server.send(200, "application/json", response.c_str());
		});

		server.on("/api/setDeviceParams", HTTP_POST, [&]() {
			if (!server.authenticate(username, password.c_str()))
				return;
			
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);
		
			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters())
					parameter.value = std::string(server.arg(parameter.id.c_str()).c_str());
			}
		});

		server.on("/api/scanNetworks", HTTP_GET, [&](){
			std::string json;
			json.reserve(64);
			json += "[";
			int scanResult{WiFi.scanComplete()};
			if(scanResult == -2)
			{
				WiFi.scanNetworks(true);
			} 
			else if (scanResult)
			{
				for (int i = 0; i < scanResult; ++i)
				{
					if(i) 
						json += ",";

					json += "{";
					json += "\"rssi\":"+ksf::to_string(WiFi.RSSI(i));
					json += ",\"ssid\":\""+std::string(WiFi.SSID(i).c_str())+"\"";
					json += ",\"bssid\":\""+std::string(WiFi.BSSIDstr(i).c_str())+"\"";
					json += ",\"channel\":"+ksf::to_string(WiFi.channel(i));
					json += ",\"secure\":"+ksf::to_string(WiFi.encryptionType(i));
					json += ",\"hidden\":"+ksf::to_string(WiFi.isHidden(i));
					json += "}";
				}
				WiFi.scanDelete();

				if(WiFi.scanComplete() == -2)
					WiFi.scanNetworks(true);
			}
			json += "]";
			server.send(200, "application/json", json.c_str());
		});

		server.on("/api/goToConfigMode", HTTP_GET, [&](){
			if (!server.authenticate(username, password.c_str()))
				return server.requestAuthentication();

			breakApp = true;
		});

#if defined(ESP8266)
		httpUpdater.setup(&server, "/api/flash", username, password.c_str());

		Update.onStart([this]() {
			onUpdateStart->broadcast();
		});

		Update.onEnd([this]() {
			updateFinished();
		});
#elif defined(ESP32)
		server.on("/api/flash", HTTP_POST, [&]() {
			if (!server.authenticate(username, password.c_str()))
				return;

			server.sendHeader("Connection", "close");
			server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
			
			updateFinished();

			delay(100);
			yield();
			delay(100);
			ESP.restart();
		}, [&]() {
			if (!server.authenticate(username, password.c_str()))
				return;

			HTTPUpload& upload = server.upload();
			if (upload.status == UPLOAD_FILE_START)
			{
				if (upload.name == "filesystem")
					Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS);
				else
					Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH);

				onUpdateStart->broadcast();
			}
			else if (upload.status == UPLOAD_FILE_WRITE)
			{
				Update.write(upload.buf, upload.currentSize);
			}
			else if (upload.status == UPLOAD_FILE_END) 
			{
				Update.end(true);
			}
		});
#endif
		server.begin();
	}
#endif

	bool ksDevicePortal::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

#if SUPPORT_HTTP_OTA
		/* Handle HTTP stuff. */
		server.handleClient();

		if (breakApp)
			return false;
#endif

		return true;
	}
}
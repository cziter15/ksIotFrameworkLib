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
    #include "ESP8266WiFi.h"
    #include "ESPAsyncTCP.h"
    #include "flash_hal.h"
    #include "FS.h"
	#define HARDWARE "ESP8266"
#elif defined(ESP32)
    #include "WiFi.h"
    #include "AsyncTCP.h"
    #include "Update.h"
    #include "esp_int_wdt.h"
    #include "esp_task_wdt.h"
	#define HARDWARE "ESP32"
#endif
#include "ESPAsyncWebServer.h"

AsyncWebServer server(80);
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
		//static const char* username{"admin"};
		server.onNotFound([&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
			response->addHeader("Location", "/");
			request->send(response);
		});

		server.on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response 
			{ 
				request->beginResponse_P(200, "text/html", DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE)
			};

			response->addHeader("Content-Encoding", "gzip");
			request->send(response);
		});

		server.on("/api/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
			std::string json;
			json.reserve(64);
			json += "{ \"id\": \"";
			json += WiFi.getHostname();
			json += "\", \"hardware\": \"" HARDWARE "\" }";
			request->send(200, "application/json", json.c_str());
		});

		server.on("/api/goToConfigMode", HTTP_GET, [&](AsyncWebServerRequest *request) {
			breakApp = true;
			request->send(200, "application/json", "{}");
		});

		server.on("/api/setDeviceParams", HTTP_GET, [&](AsyncWebServerRequest *request) {
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

			request->send(200, "application/json", response.c_str());
		});

		server.on("/api/getDeviceParams", HTTP_GET, [&](AsyncWebServerRequest *request) {
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);
		
			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters())
					parameter.value = std::string(request->arg(parameter.id.c_str()).c_str());
			}
		});

		server.on("/api/scanNetworks", HTTP_GET, [&](AsyncWebServerRequest *request) {
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
			request->send(200, "application/json", json.c_str());
		});

		server.on("/update", HTTP_POST, [&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = 
				request->beginResponse((Update.hasError())?500:200, "text/plain", (Update.hasError())?"FAIL":"OK");

			response->addHeader("Connection", "close");
			response->addHeader("Access-Control-Allow-Origin", "*");
			request->send(response);
		}, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
			if (!index) 
			{
				if(!request->hasParam("MD5", true)) 
					return request->send(400, "text/plain", "MD5 parameter missing");

				if(!Update.setMD5(request->getParam("MD5", true)->value().c_str())) 
					return request->send(400, "text/plain", "MD5 parameter invalid");

				#if defined(ESP8266)
					int cmd = (filename == "filesystem") ? U_FS : U_FLASH;
					Update.runAsync(true);
					size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
					uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
					if (!Update.begin((cmd == U_FS)?fsSize:maxSketchSpace, cmd)){ // Start with max available size
				#elif defined(ESP32)
					int cmd = (filename == "filesystem") ? U_SPIFFS : U_FLASH;
					if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
				#endif
					return request->send(400, "text/plain", "OTA could not begin");
				}
			}

			// Write chunked data to the free sketch space
			if(len)
				if (Update.write(data, len) != len)
					return request->send(400, "text/plain", "OTA could not begin");
				
			if (final) 
			{ 
				if (!Update.end(true)) 
				{
					return request->send(400, "text/plain", "Could not end OTA");
				}
			}
			else return;
		});
		server.begin();
	}
#endif

	bool ksDevicePortal::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

#if SUPPORT_HTTP_OTA
		if (breakApp)
			return false;
#endif

		return true;
	}
}
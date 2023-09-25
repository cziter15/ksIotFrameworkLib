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
#include "../res/otaWebpage.h"

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

		Serial.begin(9600);
	}
	
	bool ksDevicePortal::init(ksApplication* owner)
	{
		this->owner = owner;

		ArduinoOTA.setHostname(WiFi.getHostname());
		ArduinoOTA.begin();

		return true;
	}

	bool ksDevicePortal::postInit(ksApplication* owner)
	{
		setupUpdateWebServer();
		return true;
	}

	void ksDevicePortal::updateFinished()
	{
		ksf::saveOtaBootIndicator();
		onUpdateEnd->broadcast();
	}

	void ksDevicePortal::setupUpdateWebServer()
	{
		server = std::make_shared<AsyncWebServer>(80);

		//static const char* username{"admin"};
		server->onNotFound([&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(302, "text/plain", "");
			response->addHeader("Location", "/");
			request->send(response);
		});

		server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response 
			{ 
				request->beginResponse_P(200, "text/html", DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE)
			};

			response->addHeader("Content-Encoding", "gzip");
			request->send(response);
		});

		server->on("/api/identity", HTTP_GET, [&](AsyncWebServerRequest *request) {
			String json;
			json += "{ \"id\": \"";
			json += WiFi.getHostname();
			json += "\", \"hardware\": \"" HARDWARE "\" }";
			request->send(200, "application/json", json);
		});

		server->on("/api/getMode", HTTP_GET, [&](AsyncWebServerRequest *request) {
			bool isAPMode = WiFi.getMode() == WIFI_AP;
			String json;
			json += "{ \"isAPMode\": ";
			json += isAPMode ? "true" : "false";
			json += " }";
			request->send(200, "application/json", json);
		});

		server->on("/api/goToConfigMode", HTTP_GET, [&](AsyncWebServerRequest *request) {
			breakApp = true;
			request->send(200, "application/json", "{}");
		});

		server->on("/api/getDeviceParams", HTTP_GET, [this](AsyncWebServerRequest *request) {
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);

			String json;
			json += "{\"ssid\":\"";
			json += WiFi.SSID();
			json += "\", \"password\":\"";
			json += WiFi.psk();
			json += "\",\"params\": [";

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;
				
				configCompSp->getParameters().clear();
				configCompSp->readParams();

				for (auto& parameter : configCompSp->getParameters())
				{
					json += "{";
					json += "\"id\": \"";
					json += String(parameter.id.c_str());
					json += "\", \"value\": \"";\

					if (parameter.value.empty())
						json += String(parameter.defaultValue.c_str());
					else
						json += String(parameter.value.c_str());

					json += "\"},";
				}
			}

			if (json.endsWith(","))
				json.remove(json.length() - 1, 1);

			json += "]}";

			request->send(200, "application/plain", json);
		});

		server->on("/api/saveConfig", HTTP_POST, [this](AsyncWebServerRequest *request) {
			String ssid = request->arg("ssid");
			String password = request->arg("password");

			if (ssid.isEmpty())
			{
				request->send(200, "application/plain", "{ \"result\" : \"Empty SSID\" }");
				return;
			}
				
			WiFi.persistent(true);
			WiFi.begin(ssid.c_str(), password.c_str(), 0, nullptr, false);
			
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters())
				{
					String value{request->arg(String("param_") + parameter.id.c_str())};
					if (value.isEmpty())
						continue;

					parameter.value = value.c_str();
				}

				configCompSp->saveParams();
			}

			request->send(200, "application/plain", "{ \"result\": \"OK\" }");

			ESP.restart();
		});

		server->on("/api/scanNetworks", HTTP_GET, [&](AsyncWebServerRequest *request) {
			WiFi.scanNetworksAsync([=](int n) 
			{
				String json{"["};
				for (int i{0}; i < n; ++i)
				{
					if (i > 0)
						json += ",";
					json += "{";
					json += "\"rssi\":"+String(WiFi.RSSI(i));
					json += ",\"ssid\":\""+WiFi.SSID(i)+"\"";
					json += ",\"bssid\":\""+WiFi.BSSIDstr(i)+"\"";
					json += ",\"channel\":"+String(WiFi.channel(i));
					json += ",\"secure\":"+String(WiFi.encryptionType(i));
					json += ",\"hidden\":"+String(WiFi.isHidden(i));
					json += "}";
				}
				json += "]";
				WiFi.scanDelete();
				request->send(200, "text/plain", json);
			});
		});

		server->on("/flash", HTTP_POST, [&](AsyncWebServerRequest *request) 
		{
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

		server->begin();
	}


	bool ksDevicePortal::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		if (breakApp)
			return false;

		return true;
	}
}
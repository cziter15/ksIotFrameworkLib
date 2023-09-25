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
#include <DNSServer.h>
#include "ESPAsyncWebServer.h"
#include "../res/otaWebpage.h"

#define DP_PSTR(x) (String(FPSTR(x)).c_str())
namespace ksf::comps
{
	ksDevicePortal::ksDevicePortal()
		: ksDevicePortal(PGM_("ota_ksiotframework"))
	{}

	ksDevicePortal::ksDevicePortal(const std::string& password)
	{
		ArduinoOTA.setPassword(password.c_str());
		this->password = password;

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

		return true;
	}

	bool ksDevicePortal::postInit(ksApplication* owner)
	{
		setupUpdateWebServer();

		if (WiFi.getMode() == WIFI_AP) 
		{
			dnsServer = std::make_shared<DNSServer>();
			dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
			dnsServer->start(53, "*", WiFi.softAPIP());
		}

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

		server->onNotFound([&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = request->beginResponse(302, FPSTR("text/plain"), "");
			response->addHeader(FPSTR("Location"), "/");
			request->send(response);
		});

		server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response 
			{ 
				request->beginResponse_P(200, FPSTR("text/html"), DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE)
			};

			response->addHeader(FPSTR("Content-Encoding"), FPSTR("gzip"));
			request->send(response);
		});

		server->on(DP_PSTR("/api/identity"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			String json;
			json += FPSTR("{ \"id\": \"");
			json += WiFi.getHostname();
			json += FPSTR("\", \"hardware\": \"" HARDWARE "\" }");
			request->send(200, "application/json", json);
		});

		server->on(DP_PSTR("/api/getMode"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			bool isAPMode = WiFi.getMode() == WIFI_AP;
			String json;
			json += FPSTR("{ \"isAPMode\": ");
			json += isAPMode ? FPSTR("true") : FPSTR("false");
			json += FPSTR(" }");
			request->send(200, FPSTR("application/json"), json);
		});

		server->on(DP_PSTR("/api/goToConfigMode"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			breakApp = true;
			request->send(200, FPSTR("application/json"), "{}");
		});

		server->on(DP_PSTR("/api/getDeviceParams"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);

			String json;
			json += FPSTR("{\"ssid\":\"");
			json += WiFi.SSID();
			json += FPSTR("\", \"password\":\"");
			json += WiFi.psk();
			json += FPSTR("\",\"params\": [");

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;
				
				configCompSp->getParameters().clear();
				configCompSp->readParams();

				for (auto& parameter : configCompSp->getParameters())
				{
					json += FPSTR("{\"id\": \"");
					json += String(parameter.id.c_str());
					json += FPSTR("\", \"value\": \"");

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

			request->send(200, FPSTR("application/json"), json);
		});

		server->on(DP_PSTR("/api/saveConfig"), HTTP_POST, [&](AsyncWebServerRequest *request) {
			String ssid = request->arg(FPSTR("ssid"));
			String password = request->arg(FPSTR("password"));

			if (ssid.isEmpty())
			{
				request->send(200, FPSTR("application/json"), FPSTR("{ \"result\" : \"Empty SSID\" }"));
				return;
			}
				
			WiFi.persistent(true);
			WiFi.begin(ssid.c_str(), password.c_str(), 0, nullptr, false);
			
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);

			String paramPrefix{FPSTR("param_")};

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters())
				{
					String value{request->arg(paramPrefix + parameter.id.c_str())};
					if (value.isEmpty())
						continue;

					parameter.value = value.c_str();
				}

				configCompSp->saveParams();
			}

			request->send(200, FPSTR("application/json"), FPSTR("{ \"result\": \"OK\" }"));

			delay(500);
			ESP.restart();
		});

		server->on(DP_PSTR("/api/scanNetworks"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			switch (WiFi.scanComplete())
			{
				case WIFI_SCAN_FAILED:
					WiFi.scanNetworks(true);
				case WIFI_SCAN_RUNNING:
					request->send(500, FPSTR("text/plain"), "");
				return;

				default:
				{
					String json;
					json += '[';
					for (int i{0}; i < WiFi.scanComplete(); ++i)
					{
						if (i > 0)
							json += ',';
						json += FPSTR("{\"rssi\":")+String(WiFi.RSSI(i));
						json += FPSTR(",\"ssid\":\"")+WiFi.SSID(i)+"\"";
						json += FPSTR(",\"bssid\":\"")+WiFi.BSSIDstr(i)+"\"";
						json += FPSTR(",\"channel\":")+String(WiFi.channel(i));
						json += FPSTR(",\"secure\":")+String(WiFi.encryptionType(i));
						json += '}';
					}
					json +=	']';
					WiFi.scanDelete();
					request->send(200, FPSTR("application/json"), json);
				}
			}
		});

		server->on(DP_PSTR("/api/flash"), HTTP_POST, [&](AsyncWebServerRequest *request) {
			AsyncWebServerResponse *response = 
				request->beginResponse((Update.hasError())?500:200, FPSTR("text/plain"), (Update.hasError())?"FAIL":"OK");

			response->addHeader(FPSTR("Connection"), FPSTR("close"));
			response->addHeader(FPSTR("Access-Control-Allow-Origin"), "*");
			request->send(response);
			ESP.restart();
		}, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
			if (!index) 
			{
				if(!request->hasParam("MD5", true)) 
					return request->send(400, FPSTR("text/plain"), FPSTR("MD5 parameter missing"));

				if(!Update.setMD5(request->getParam("MD5", true)->value().c_str())) 
					return request->send(400, FPSTR("text/plain"),  FPSTR("MD5 parameter invalid"));

				#if defined(ESP8266)
					int cmd = U_FLASH;
					Update.runAsync(true);
					size_t fsSize = ((size_t) &_FS_end - (size_t) &_FS_start);
					uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
					if (!Update.begin((cmd == U_FS)?fsSize:maxSketchSpace, cmd)){ // Start with max available size
				#elif defined(ESP32)
					int cmd = U_FLASH;
					if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) { // Start with max available size
				#endif
					return request->send(400,  FPSTR("text/plain"), FPSTR("OTA failed to start"));
				}
			}

			// Write chunked data to the free sketch space
			if(len)
				if (Update.write(data, len) != len)
						return request->send(400,  FPSTR("text/plain"), FPSTR("OTA failed to start"));
				
			if (final) 
			{
				if (!Update.end(true)) 
					return request->send(400,  FPSTR("text/plain"), FPSTR("OTA failed to start"));
			}
			else return;
		});

		server->begin();
	}


	bool ksDevicePortal::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		if (dnsServer)
			dnsServer->processNextRequest();

		if (breakApp)
			return false;

		return true;
	}
}
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
	#include "esp_wifi.h"
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
			String acceptHeader{request->header(FPSTR("Accept"))};
			if (acceptHeader.indexOf(FPSTR("text/html")) == -1) 
			{
				request->send(404, FPSTR("text/html"), FPSTR("Not found"));
				return;
			}
			auto response{request->beginResponse(302, FPSTR("text/plain"), "")};
			response->addHeader(FPSTR("Location"), "/");
			request->send(response);
		});

		server->on("/", HTTP_GET, [&](AsyncWebServerRequest *request) {
			auto response{request->beginResponse_P(200, FPSTR("text/html"), DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE)};
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

		server->on(DP_PSTR("/api/goToConfigMode"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			breakApp = true;
		});

		server->on(DP_PSTR("/api/getDeviceParams"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);
			bool isInConfigMode{!configCompsWp.empty()};

			String json;
			json += FPSTR("{\"isConfigMode\": ");
			json += isInConfigMode ? FPSTR("true") : FPSTR("false");

			if (!isInConfigMode)
			{
				json += '}';
				request->send(200, FPSTR("application/json"), json);
				return;
			}
			#ifdef ESP8266
				struct station_config conf;
				wifi_station_get_config_default(&conf);
				auto ssid = reinterpret_cast<const char*>(conf.ssid);
				auto pass = reinterpret_cast<const char*>(conf.password);
			#elif defined(ESP32)
				wifi_config_t conf;
				esp_wifi_get_config(WIFI_IF_STA, &conf);
				auto ssid = reinterpret_cast<const char*>(conf.sta.ssid);
				auto pass = reinterpret_cast<const char*>(conf.sta.password);
			#endif

			String ssidString;
			ssidString.reserve(32);
			for (auto idx{0}; idx < 32; ++idx)
			{
				if (ssid[idx] == '\0')
					break;
				
				ssidString += ssid[idx];
			}

			String passString;
			passString.reserve(64);
			for (auto idx{0}; idx < 64; ++idx)
			{
				if (pass[idx] == '\0')
					break;

				passString += pass[idx];
			}

			json += FPSTR(",\"ssid\":\"");
			json += ssidString;
			json += FPSTR("\", \"password\":\"");
			json += passString;
			json += FPSTR("\",\"params\": [");

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;
				
				auto& paramListRef{configCompSp->getParameters()};
				paramListRef.clear();
				configCompSp->readParams();

				for (auto paramRef : paramListRef)
				{
					json += FPSTR("{\"id\": \"");
					json += String(paramRef.id.c_str());
					json += FPSTR("\", \"value\": \"");
					json += String(paramRef.value.c_str());
					json += FPSTR("\", \"default\": \"");
					json += String(paramRef.defaultValue.c_str());
					json += FPSTR("\"},");
				}
			}

			if (json.endsWith(","))
				json = json.substring(0, json.length() - 1);
				
			json += "]}";

			request->send(200, FPSTR("application/json"), json);
		});

		server->on(DP_PSTR("/api/saveConfig"), HTTP_POST, [&](AsyncWebServerRequest *request) {
			auto& ssid{request->getParam(FPSTR("ssid"), true)->value()};
			auto& password{request->getParam(FPSTR("password"), true)->value()};

			if (ssid.isEmpty())
			{
				request->send(200, FPSTR("application/json"), FPSTR("{ \"result\" : \"Empty SSID\" }"));
				return;
			}

			WiFi.persistent(true);
			WiFi.begin(ssid.c_str(), password.c_str(), 0, 0, false);
			WiFi.persistent(false);
			
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
					auto value{request->getParam(paramPrefix + parameter.id.c_str(), true)->value()};
					if (value.isEmpty())
						continue;

					parameter.value = value.c_str();
				}

				configCompSp->saveParams();
			}

			request->send(200, FPSTR("application/json"), FPSTR("{ \"result\": \"OK\" }"));
			delay(1000);
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
						json += FPSTR("{\"rssi\":");
						json += String(WiFi.RSSI(i));
						json += FPSTR(",\"ssid\":\"");
						json += WiFi.SSID(i)+"\"";
						json += FPSTR(",\"bssid\":\"");
						json += WiFi.BSSIDstr(i)+"\"";
						json += FPSTR(",\"channel\":");
						json += String(WiFi.channel(i));
						json += FPSTR(",\"secure\":");
						json += String(WiFi.encryptionType(i));
						json += '}';
					}
					json +=	']';
					WiFi.scanDelete();
					request->send(200, FPSTR("application/json"), json);
				}
			}
		});

		server->on(DP_PSTR("/api/flash"), HTTP_POST, [&](AsyncWebServerRequest *request) {
			auto statusCode{(Update.hasError())?500:200};
			auto response{request->beginResponse(statusCode, FPSTR("text/plain"), (Update.hasError())?"FAIL":"OK")};
			response->addHeader(FPSTR("Connection"), FPSTR("close"));
			response->addHeader(FPSTR("Access-Control-Allow-Origin"), "*");
			request->send(response);
			updateFinished();
			delay(1000);
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
		{
			/*
				Well it looks like that component destroy order has critical
				impact on underlying services so we need to cleanup dnsServer
				and web server earlier than in destructor.
			*/
			dnsServer = nullptr;
			server = nullptr;

			return false;
		}

		return true;
	}
}
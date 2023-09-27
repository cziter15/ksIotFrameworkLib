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

#include <string_view>

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
	class StdStrWebResponse
	{
		private:
			std::string content;
		public:
			StdStrWebResponse(std::string&& content) {
				this->content = std::move(content);
			}
			
			AsyncCallbackResponse* operator*() const
			{
				auto type{FPSTR("text/plain")};
				auto cs{content.length()};
				return new AsyncCallbackResponse(type, cs, [&, cs](uint8_t *buf, size_t maxl, size_t idx) {
					return content.copy((char *)buf, std::min(maxl, cs - idx), idx);
				});
			}
	};

	ksDevicePortal::ksDevicePortal()
		: ksDevicePortal(PGM_("ota_ksiotframework"))
	{}

	ksDevicePortal::ksDevicePortal(const std::string& password)
	{
		this->password = password;
		ArduinoOTA.setPassword(password.c_str());

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

		auto isAuthorized = [](ksDevicePortal* portal, AsyncWebServerRequest *request) {
			if (WiFi.getMode() == WIFI_AP)
				return true;
			if (portal->password.empty() || request->authenticate("admin", portal->password.c_str()))
				return true;
			request->requestAuthentication();
			return false;
		};

		#define REQUIRE_AUTH() if (!isAuthorized(this, request)) return;

		if (WiFi.getMode() & WIFI_AP)
		{
			server->on(DP_PSTR("/api/saveConfig"), HTTP_POST, [&](AsyncWebServerRequest *request) {
				REQUIRE_AUTH()

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
						String param_id{paramPrefix + parameter.id.c_str()};
						auto value{request->getParam(param_id, true)->value()};
						if (value.isEmpty())
							continue;

						parameter.value = value.c_str();
					}

					configCompSp->saveParams();
				}

				request->send(200, FPSTR("application/json"), FPSTR("{ \"result\": \"OK\" }"));

				server->reset();
				breakRequestMillis = millis();
			});

			server->on(DP_PSTR("/api/scanNetworks"), HTTP_GET, [&](AsyncWebServerRequest *request) {
				REQUIRE_AUTH()

				switch (WiFi.scanComplete())
				{
					case WIFI_SCAN_FAILED:
						WiFi.scanNetworks(true);
					case WIFI_SCAN_RUNNING:
						request->send(304);
					return;

					default:
					{
						std::string json;
						json += '[';
						for (int i{0}; i < WiFi.scanComplete(); ++i)
						{
							if (i > 0)
								json += ',';
							json += PGM_("{\"rssi\":");
							json += ksf::to_string(WiFi.RSSI(i));
							json += PGM_(",\"ssid\":\"");
							json += WiFi.SSID(i).c_str();
							json += PGM_("\",\"bssid\":\"");
							json += WiFi.BSSIDstr(i).c_str();
							json += PGM_("\",\"channel\":");
							json += ksf::to_string(WiFi.channel(i));
							json += PGM_(",\"secure\":");
							json += ksf::to_string(WiFi.encryptionType(i));
							json += '}';
						}
						json +=	']';
						WiFi.scanDelete();

						request->send(*StdStrWebResponse(std::move(json)));
					}
				}
			});
		}

		if (WiFi.getMode() & WIFI_STA)
		{
			server->on(DP_PSTR("/api/goToConfigMode"), HTTP_GET, [&](AsyncWebServerRequest *request) {
				REQUIRE_AUTH()
				breakRequestMillis = millis();
				request->send(200);
			});
		}

		server->on(DP_PSTR("/api/online"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			bool busy{breakRequestMillis != 0 || rebootRequestMillis != 0};
			request->send(busy ? 503 : 200);
		});

		server->onNotFound([&](AsyncWebServerRequest *request) {
			REQUIRE_AUTH()

			auto acceptHeader{request->header(FPSTR("Accept"))};
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
			REQUIRE_AUTH()

			auto response{request->beginResponse_P(200, FPSTR("text/html"), DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE)};
			response->addHeader(FPSTR("Content-Encoding"), FPSTR("gzip"));
			request->send(response);
		});

		server->on(DP_PSTR("/api/getIdentity"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			std::string json;
			json += PGM_("[{\"name\":\"Hardware\",\"value\":\"");
			json += HARDWARE;
			json += PGM_("\"},{\"name\":\"Hostname\",\"value\":\"");
			json += WiFi.getHostname();
			json += PGM_("\"},{\"name\":\"Device uptime\",\"value\":\"");
			json += ksf::getUptimeString();
			json += PGM_("\"},{\"name\":\"Reset reason\",\"value\":\"");
			json += ksf::getResetReason();
			json += PGM_("\"},{\"name\":\"IP address\",\"value\":\"");
			json += WiFi.getMode() == WIFI_AP ?  WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str();
			json += PGM_("\"}]");
			request->send(*StdStrWebResponse(std::move(json)));
		});

		server->on(DP_PSTR("/api/getDeviceParams"), HTTP_GET, [&](AsyncWebServerRequest *request) {
			REQUIRE_AUTH()

			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);
			bool isInConfigMode{!configCompsWp.empty()};

			std::string json;
			json += PGM_("{\"isConfigMode\": ");
			json += isInConfigMode ? PGM_("true") : PGM_("false");

			if (!isInConfigMode)
			{
				json += '}';
				request->send(*StdStrWebResponse(std::move(json)));
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

			std::string_view ssid_sv{ssid, strnlen(ssid, 32)};
			std::string_view pass_sv{pass, strnlen(pass, 64)};
		
			json += PGM_(",\"ssid\":\"");
			json += ssid_sv;
			json += PGM_("\", \"password\":\"");
			json += pass_sv;
			json += PGM_("\",\"params\": [");

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
					json += PGM_("{\"id\": \"");
					json += paramRef.id;
					json += PGM_("\", \"value\": \"");
					json += paramRef.value;
					json += PGM_("\", \"default\": \"");
					json += paramRef.defaultValue;
					json += PGM_("\"},");
				}
			}

			if (json.back() == ',')
				json.pop_back();
			
			json += "]}";

			request->send(*StdStrWebResponse(std::move(json)));
		});

		server->on(DP_PSTR("/api/flash"), HTTP_POST, [&](AsyncWebServerRequest *request) {
			REQUIRE_AUTH()

			auto statusCode{(Update.hasError())?500:200};
			auto response{request->beginResponse(statusCode, FPSTR("text/plain"), (Update.hasError())?"FAIL":"OK")};
			response->addHeader(FPSTR("Connection"), FPSTR("close"));
			response->addHeader(FPSTR("Access-Control-Allow-Origin"), "*");
			request->send(response);
			updateFinished();
			rebootRequestMillis = millis();
		}, [&](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
			REQUIRE_AUTH()

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

		if (rebootRequestMillis != 0 && millis() - rebootRequestMillis > 1000)
			ESP.restart();

		if (breakRequestMillis != 0 && millis() - breakRequestMillis > 1000)
			return false;

		return true;
	}
}
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
	#include "flash_hal.h"
    #include "ESP8266WiFi.h"
    #include "WiFiClient.h"
    #include "ESP8266WebServer.h"
    #define WebServerClass ESP8266WebServer
	#define HARDWARE "ESP8266"
#elif defined(ESP32)
    #include "WiFi.h"
    #include "WiFiClient.h"
    #include "WebServer.h"
    #define WebServerClass WebServer
	#define HARDWARE "ESP32"
#endif

#include <DNSServer.h>

#include "../res/otaWebpage.h"

#define DP_PSTR(x) (String(FPSTR(x)).c_str())
namespace ksf::comps
{
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
			dnsServer = new DNSServer();
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
		serverRawPtr = new WebServerClass(80);
		auto server{static_cast<WebServerClass*>(serverRawPtr)};
		
		auto isAuthorized = [](ksDevicePortal* portal, WebServerClass *server) {
			if (WiFi.getMode() == WIFI_AP)
				return true;
			if (portal->password.empty() || server->authenticate("admin", portal->password.c_str()))
				return true;
			server->requestAuthentication();
			return false;
		};

		#define REQUIRE_AUTH() if (!isAuthorized(this, server)) return;

		if (WiFi.getMode() & WIFI_AP)
		{
			server->on(DP_PSTR("/api/saveConfig"), HTTP_POST, [&, server]() {
				REQUIRE_AUTH()

				auto& ssid{server->arg(FPSTR("ssid"))};

				if (ssid.isEmpty())
				{
					server->send(200, FPSTR("application/json"), FPSTR("{ \"result\" : \"Empty SSID\" }"));
					return;
				}
				
				auto& password{server->arg(FPSTR("password"))};

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
						auto value{server->arg(param_id)};
						if (value.isEmpty())
							continue;

						parameter.value = value.c_str();
					}

					configCompSp->saveParams();
				}

				ksf::saveCredentials(ssid.c_str(), password.c_str());

				server->send(200);

				breakApp = true;
			});

			server->on(DP_PSTR("/api/scanNetworks"), HTTP_GET, [&, server]() {
				REQUIRE_AUTH()

				switch (WiFi.scanComplete())
				{
					case WIFI_SCAN_FAILED:
						WiFi.scanNetworks(true);
					case WIFI_SCAN_RUNNING:
						server->send(304);
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
					
						WiFi.enableSTA(false);
						server->send(200, FPSTR("application/json"), json.c_str());
					}
				}
			});
		}

		if (WiFi.getMode() & WIFI_STA)
		{
			server->on(DP_PSTR("/api/goToConfigMode"),HTTP_GET, [&, server]() {
				REQUIRE_AUTH()
				server->send(200);
				breakApp = true;
			});
		}

		server->on(DP_PSTR("/api/online"), HTTP_GET, [&, server]() {
			server->send(200);
		});

		server->onNotFound([&, server]() {
			REQUIRE_AUTH()

			auto acceptHeader{server->header(FPSTR("Accept"))};
			if (acceptHeader.indexOf(FPSTR("text/html")) == -1) 
			{
				server->send(404, FPSTR("text/html"), FPSTR("Not found"));
				return;
			}

			server->sendHeader(FPSTR("Location"), "/");
			server->send(302);
		});

		server->on("/", HTTP_GET, [&, server]() {
			REQUIRE_AUTH()
			server->sendHeader(FPSTR("Content-Encoding"), FPSTR("gzip"));
			server->send_P(200, "text/html", (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
		});

		server->on(DP_PSTR("/api/getIdentity"), HTTP_GET, [&, server]() {
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
			server->send(200, "application/json", json.c_str());
		});

		server->on(DP_PSTR("/api/getDeviceParams"), HTTP_GET, [&, server]() {
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
				server->send(200, "application/json", json.c_str());
				return;
			}

			std::string ssid, pass;
			ksf::loadCredentials(ssid, pass);

			json += PGM_(",\"ssid\":\"");
			json += ssid;
			json += PGM_("\", \"password\":\"");
			json += pass;
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

			server->send(200, FPSTR("application/json"), json.c_str());
		});

		server->on("/api/flash", HTTP_POST, [&, server]() {
			REQUIRE_AUTH()

			server->sendHeader("Connection", "close");
			server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
			
			updateFinished();

			delay(100);
			yield();
			delay(100);
			ESP.restart();
		}, [&]() {
			REQUIRE_AUTH()

			HTTPUpload& upload = server->upload();
			if (upload.status == UPLOAD_FILE_START)
			{
				#if defined(ESP8266)
					Update.runAsync(true);
					uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
					if (!Update.begin(maxSketchSpace, U_FLASH)) {
				#elif defined(ESP32)
					if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) { // Start with max available size
				#endif
					return server->send(400, "text/plain", "OTA could not begin");
				}
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

		server->begin();
	}

	bool ksDevicePortal::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		if (breakApp)
			return false;

		if (dnsServer)
			dnsServer->processNextRequest();

		if (serverRawPtr)
			static_cast<WebServerClass*>(serverRawPtr)->handleClient();

		return true;
	}

	ksDevicePortal::~ksDevicePortal()
	{
		if (dnsServer)
		{
			delete dnsServer;
			dnsServer = nullptr;
		}

		if (serverRawPtr)
		{
			delete static_cast<WebServerClass*>(serverRawPtr);
			serverRawPtr = nullptr;
		}
	}
}
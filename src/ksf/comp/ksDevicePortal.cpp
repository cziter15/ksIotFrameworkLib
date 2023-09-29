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

#include <LittleFS.h>
#include <DNSServer.h>

#include "../res/otaWebpage.h"
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

	void ksDevicePortal::rebootDevice()
	{
		delay(100);
		ESP.restart();
	}

	void ksDevicePortal::updateFinished()
	{
		ksf::saveOtaBootIndicator();
		onUpdateEnd->broadcast();
	}

	bool ksDevicePortal::inRequest_NeedAuthentication() const
	{
		if (WiFi.getMode() == WIFI_AP)
			return false;

		if (password.empty() || serverAs<WebServerClass>()->authenticate("admin", password.c_str()))
			return false;

		serverAs<WebServerClass>()->requestAuthentication();
		return true;
	}

	void ksDevicePortal::onRequest_saveConfig()
	{
		if (inRequest_NeedAuthentication())
			return;

		auto ssid{serverAs<WebServerClass>()->arg(FPSTR("ssid"))};
		if (ssid.isEmpty())
		{
			serverAs<WebServerClass>()->send(200, FPSTR("application/json"), FPSTR("{ \"result\" : \"Empty SSID\" }"));
			return;
		}

		auto password{serverAs<WebServerClass>()->arg(FPSTR("password"))};

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
				auto value{serverAs<WebServerClass>()->arg(param_id)};
				if (value.isEmpty())
					continue;

				parameter.value = value.c_str();
			}
			configCompSp->saveParams();
		}
		ksf::saveCredentials(ssid.c_str(), password.c_str());

		serverAs<WebServerClass>()->send(200);
		breakApp = true;
	}

	void ksDevicePortal::onRequest_scanNetworks()
	{
		if (inRequest_NeedAuthentication())
			return;

		switch (WiFi.scanComplete())
		{
			case WIFI_SCAN_FAILED:
				WiFi.scanNetworks(true);
			case WIFI_SCAN_RUNNING:
				serverAs<WebServerClass>()->send(304);
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
				serverAs<WebServerClass>()->send(200, FPSTR("application/json"), json.c_str());
			}
		}
	}

	void ksDevicePortal::onRequest_getIdentity() const
	{
		if (inRequest_NeedAuthentication())
			return;

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
		serverAs<WebServerClass>()->send(200, "application/json", json.c_str());
	}

	void ksDevicePortal::onRequest_getDeviceParams() const
	{
		if (inRequest_NeedAuthentication())
			return;

		std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
		owner->findComponents<ksConfigProvider>(configCompsWp);
		bool isInConfigMode{!configCompsWp.empty()};

		std::string json;
		json += PGM_("{\"isConfigMode\": ");
		json += isInConfigMode ? PGM_("true") : PGM_("false");

		if (!isInConfigMode)
		{
			json += '}';
			serverAs<WebServerClass>()->send(200, "application/json", json.c_str());
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

		serverAs<WebServerClass>()->send(200, FPSTR("application/json"), json.c_str());
	}

	void ksDevicePortal::onRequest_goToConfigMode()
	{
		if (inRequest_NeedAuthentication())
			return;
	
		serverAs<WebServerClass>()->send(200);
		breakApp = true;
	}

	void ksDevicePortal::onRequest_online() const
	{
		if (inRequest_NeedAuthentication())
			return;

		serverAs<WebServerClass>()->send(200);
	}

	void ksDevicePortal::onRequest_notFound() const
	{
		auto server{serverAs<WebServerClass>()};

		/* For files always return 404. */
		if (server->uri().lastIndexOf('.') != -1)
		{
			server->send(404);
			return;
		}

		/* Otherwise... redirect. */
		server->sendHeader(FPSTR("Location"), "/");
		server->send(302);
	}

	void ksDevicePortal::onRequest_otaChunk()
	{
		if (inRequest_NeedAuthentication())
			return;
	
		HTTPUpload& upload = serverAs<WebServerClass>()->upload();
		if (upload.status == UPLOAD_FILE_START)
		{
#if defined(ESP8266)
			Update.runAsync(true);
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#elif defined(ESP32)
			uint32_t maxSketchSpace = UPDATE_SIZE_UNKNOWN;
#endif
			if (!Update.begin(maxSketchSpace, U_FLASH)) 
				return serverAs<WebServerClass>()->send(400, "text/plain", "OTA could not begin");

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
	}

	void ksDevicePortal::onRequest_otaFinish()
	{
		if (inRequest_NeedAuthentication())
			return;

		bool hasError{Update.hasError()};

		serverAs<WebServerClass>()->sendHeader("Connection", "close");
		serverAs<WebServerClass>()->send(200, "text/plain", hasError? "FAIL" : "OK");
		
		if (hasError)
			return;

		updateFinished();
		rebootDevice();
	}
	
	void ksDevicePortal::onRequest_index() const
	{
		if (inRequest_NeedAuthentication())
			return;

		serverAs<WebServerClass>()->sendHeader(FPSTR("Content-Encoding"), FPSTR("gzip"));
		serverAs<WebServerClass>()->send_P(200, "text/html", (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
	}

	void ksDevicePortal::onRequest_formatFS()
	{
		if (inRequest_NeedAuthentication())
			return;

		LittleFS.format();

#if defined(ESP8266)
		ESP.eraseConfig();
#elif defined(ESP32)
		WiFi.enableSTA(true);
		WiFi.persistent(true);
		WiFi.disconnect(true,true);
		delay(500);
		WiFi.persistent(false);
#endif

		rebootDevice();
	}

	void ksDevicePortal::setupUpdateWebServer()
	{
		_webServer = new WebServerClass(80);
		auto server{serverAs<WebServerClass>()};

		server->onNotFound(std::bind(&ksDevicePortal::onRequest_notFound, this));

		server->on(FPSTR("/"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_index, this));
		server->on(FPSTR("/api/online"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_online, this));
		server->on(FPSTR("/api/getIdentity"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_getIdentity, this));
		server->on(FPSTR("/api/getDeviceParams"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_getDeviceParams, this));

		if (WiFi.getMode() & WIFI_AP)
		{
			server->on(FPSTR("/api/saveConfig"), HTTP_POST, std::bind(&ksDevicePortal::onRequest_saveConfig, this));
			server->on(FPSTR("/api/scanNetworks"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_scanNetworks, this));
		}
		else
		{
			server->on(FPSTR("/api/goToConfigMode"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_goToConfigMode, this));
		}

		server->on("/api/flash", HTTP_POST, 
			std::bind(&ksDevicePortal::onRequest_otaFinish, this),	// Upload file part
			std::bind(&ksDevicePortal::onRequest_otaChunk, this)	// Upload file end
		);

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

		if (auto server{serverAs<WebServerClass>()})
			server->handleClient();

		return true;
	}

	ksDevicePortal::~ksDevicePortal()
	{
		if (dnsServer)
			delete dnsServer;

		if (auto webServer{serverAs<WebServerClass>()})
			delete webServer;
	}
}
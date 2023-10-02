/*
 *	Copyright (c) 2021-2023, Krzysztof Strehlau
 *
 *	This file is part of the ksIotFramework library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <LittleFS.h>
#include <DNSServer.h>

#if ESP8266
	#include "flash_hal.h"
	#include "ESP8266WiFi.h"
	#include "WiFiClient.h"
	#include "ESP8266WebServer.h"
	#define HARDWARE "ESP8266"
#elif ESP32
	#include "WiFi.h"
	#include "WiFiClient.h"
	#include "WebServer.h"
	#define WebServerClass WebServer
	#define HARDWARE "ESP32"
#else
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "ksWifiConnector.h"
#include "ksDevicePortal.h"
#include "ksConfigProvider.h"

#include "../res/otaWebpage.h"

namespace ksf::comps
{
	ksDevicePortal::~ksDevicePortal() = default;
	
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
			updateFinished(false);
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
			dnsServer = std::make_unique<DNSServer>();
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

	void ksDevicePortal::updateFinished(bool fromPortal)
	{
		ksf::saveOtaBootIndicator(fromPortal ? EOTAType::OTA_GENERIC : EOTAType::OTA_PORTAL);
		onUpdateEnd->broadcast();
	}

	bool ksDevicePortal::inRequest_NeedAuthentication() const
	{
		if (WiFi.getMode() == WIFI_AP)
			return false;

		if (password.empty() || webServer->authenticate(PGM_("admin").c_str(), password.c_str()))
			return false;

		webServer->requestAuthentication();
		return true;
	}

	void ksDevicePortal::onRequest_saveConfig()
	{
		if (inRequest_NeedAuthentication())
			return;

		auto ssid{webServer->arg(FPSTR("ssid"))};
		if (ssid.isEmpty())
		{
			webServer->send(200, FPSTR("application/json"), FPSTR("{ \"result\" : \"Empty SSID\" }"));
			return;
		}

		auto password{webServer->arg(FPSTR("password"))};

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
				auto value{webServer->arg(param_id)};
				if (value.isEmpty())
					continue;

				parameter.value = value.c_str();
			}
			configCompSp->saveParams();
		}
		ksf::saveCredentials(ssid.c_str(), password.c_str());

		webServer->send(200);
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
				webServer->send(304);
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
				webServer->send(200, FPSTR("application/json"), json.c_str());
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
		webServer->send(200, FPSTR("application/json"), json.c_str());
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
			webServer->send(200, FPSTR("application/json"), json.c_str());
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

		webServer->send(200, FPSTR("application/json"), json.c_str());
	}

	void ksDevicePortal::onRequest_goToConfigMode()
	{
		if (inRequest_NeedAuthentication())
			return;
	
		webServer->send(200);
		breakApp = true;
	}

	void ksDevicePortal::onRequest_online() const
	{
		if (inRequest_NeedAuthentication())
			return;

		webServer->send(200);
	}

	void ksDevicePortal::onRequest_notFound() const
	{
		/* For files always return 404. */
		if (webServer->uri().lastIndexOf('.') != -1)
		{
			webServer->send(404);
			return;
		}

		/* Otherwise... redirect. */
		webServer->sendHeader(FPSTR("Location"), "/");
		webServer->send(302);
	}

	void ksDevicePortal::onRequest_otaChunk()
	{
		if (inRequest_NeedAuthentication())
			return;
	
		HTTPUpload& upload = webServer->upload();
		if (upload.status == UPLOAD_FILE_START)
		{
#if defined(ESP8266)
			Update.runAsync(true);
			uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
#elif defined(ESP32)
			uint32_t maxSketchSpace = UPDATE_SIZE_UNKNOWN;
#endif
			if (!Update.begin(maxSketchSpace, U_FLASH)) 
				return webServer->send(400, FPSTR("text/plain"), FPSTR("OTA could not begin"));

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

		webServer->sendHeader(FPSTR("Connection"), FPSTR("close"));
		webServer->send(200, FPSTR("text/plain"), hasError ? PGM_("FAIL").c_str() : PGM_("OK").c_str());

		if (hasError)
			return;

		updateFinished(true);
		rebootDevice();
	}
	
	void ksDevicePortal::onRequest_index() const
	{
		if (inRequest_NeedAuthentication())
			return;

		webServer->sendHeader(FPSTR("Content-Encoding"), FPSTR("gzip"));
		webServer->send_P(200, PGM_("text/html").c_str(), (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
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
		webServer = std::make_unique<WebServerClass>(80);

		webServer->onNotFound(std::bind(&ksDevicePortal::onRequest_notFound, this));

		webServer->on(FPSTR("/"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_index, this));
		webServer->on(FPSTR("/api/online"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_online, this));
		webServer->on(FPSTR("/api/getIdentity"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_getIdentity, this));
		webServer->on(FPSTR("/api/getDeviceParams"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_getDeviceParams, this));

		if (WiFi.getMode() & WIFI_AP)
		{
			webServer->on(FPSTR("/api/saveConfig"), HTTP_POST, std::bind(&ksDevicePortal::onRequest_saveConfig, this));
			webServer->on(FPSTR("/api/scanNetworks"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_scanNetworks, this));
		}
		else
		{
			webServer->on(FPSTR("/api/goToConfigMode"), HTTP_GET, std::bind(&ksDevicePortal::onRequest_goToConfigMode, this));
		}

		webServer->on("/api/flash", HTTP_POST, 
			std::bind(&ksDevicePortal::onRequest_otaFinish, this),	// Upload file part
			std::bind(&ksDevicePortal::onRequest_otaChunk, this)	// Upload file end
		);

		webServer->begin();
	}

	bool ksDevicePortal::loop()
	{
		if (breakApp)
			return false;

		/* Handle OTA stuff. */
		ArduinoOTA.handle();

		/* Handle requests. */
		if (dnsServer)
			dnsServer->processNextRequest();

		/* Handle web server. */
		if (webServer)
			webServer->handleClient();

		return true;
	}
}
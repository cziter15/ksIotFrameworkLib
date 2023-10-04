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
#include <WebSocketsServer.h>
#include <map>

#if ESP8266
	#include "flash_hal.h"
	#include "ESP8266WiFi.h"
	#include "WiFiClient.h"
	#include "ESP8266WebServer.h"
	#define HARDWARE "ESP8266"

	uint8_t ESP_getFlashVendor() { return ESP.getFlashChipVendorId(); }
	uint32_t ESP_getFlashSizeKB() { return ESP.getFlashChipRealSize()/1024; }
#elif ESP32
	#include "WiFi.h"
	#include "WiFiClient.h"
	#include "WebServer.h"
	#define WebServerClass WebServer
	#define HARDWARE "ESP32"

	extern uint32_t ESP_getFlashChipId(void);
	uint8_t ESP_getFlashVendor() { return ESP_getFlashChipId() & 0xFF; }
	uint32_t ESP_getFlashSizeKB() { return ESP.magicFlashChipSize(ESP.getFlashChipSize())/1024; }
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
	const char PROGMEM_TEXT_PLAIN[] PROGMEM {"text/plain"};
	const char PROGMEM_APPLICATION_JSON [] PROGMEM {"application/json"};
	const char PROGMEM_TEXT_HTML [] PROGMEM {"text/html"};
	const char PROGMEM_ACCEPT [] PROGMEM {"Accept"};
	const char PROGMEM_IF_NONE_MATCH [] PROGMEM {"If-None-Match"};

	ksDevicePortal::~ksDevicePortal()
	{
		if (webSocket)
			webSocket->close();
	}
	
	ksDevicePortal::ksDevicePortal()
		: ksDevicePortal(PSTR("ota_ksiotframework"))
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
		ksf::saveOtaBootIndicator(fromPortal ? EOTAType::OTA_PORTAL : EOTAType::OTA_GENERIC);
		onUpdateEnd->broadcast();
	}

	bool ksDevicePortal::inRequest_NeedAuthentication() const
	{
		if (WiFi.getMode() == WIFI_AP)
			return false;

		if (password.empty() || webServer->authenticate(PSTR("admin"), password.c_str()))
			return false;

		webServer->requestAuthentication();
		return true;
	}

	void ksDevicePortal::onWebsocketTextMessage(uint8_t clientNum, std::string_view message)
	{
		auto idEnd{message.find('|', 0)};
		if (idEnd == std::string_view::npos)
			return;

		auto commandStart{idEnd + 1};
		auto commandEnd{message.find('|', commandStart + 1)};

		if (commandEnd == std::string_view::npos)
			return;

		auto id{message.substr(0, idEnd)};
		auto command{message.substr(commandStart, commandEnd - commandStart)};
		auto body{message.substr(commandEnd + 1)};

		std::string response{id};
		response += '|';
		
		if (command == PSTR("getIdentity"))
		{
			handle_getIdentity(response);
		}
		else if (command == PSTR("scanNetworks"))
		{
			handle_scanNetworks(response);
		}
		else if (command == PSTR("getDeviceParams"))
		{
			handle_getDeviceParams(response);
		}
		else if (command == PSTR("goToConfigMode"))
		{
			requestAppBreak();
			return;
		}
		else if (command == PSTR("saveConfig"))
		{
			// Split the input into lines
			std::map<std::string_view, std::string_view> paramMap;

			size_t startPos = 0;
			size_t endPos = body.find('\n');

			while (endPos != std::string_view::npos) 
			{
				std::string_view line{body.substr(startPos, endPos - startPos)};
				size_t equalPos{line.find('=')};

				if (equalPos != std::string_view::npos) 
					paramMap[line.substr(0, equalPos)] = line.substr(equalPos + 1);

				startPos = endPos + 1;
				endPos = body.find('\n', startPos);
			}

			std::string ssid{paramMap[PSTR("ssid")]};
			std::string password{paramMap[PSTR("password")]};

			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			owner->findComponents<ksConfigProvider>(configCompsWp);
			std::string paramPrefix{};

			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;
				for (auto& parameter : configCompSp->getParameters())
				{
					std::string param_id{PSTR("param_") + parameter.id};
					auto param_it{paramMap.find(param_id)};
					if (param_it != paramMap.end())
						parameter.value = param_it->second;
				}
				configCompSp->saveParams();
			}

			ksf::saveCredentials(ssid, password);
			requestAppBreak();
			return;
		}

		webSocket->sendTXT(clientNum, response.c_str(), response.size());
	}

	void ksDevicePortal::handle_getIdentity(std::string& response)
	{
		response += PSTR("[{\"name\":\"MCU chip\",\"value\":\"");
		response += HARDWARE " (";
		response += ksf::to_string(ESP.getCpuFreqMHz());
		response += PSTR(" MHz)\"},{\"name\":\"Flash chip\",\"value\":\"");
		response += "VID:";
		response += ksf::to_string(ESP_getFlashVendor());
		response += PSTR(" (");
		response += ksf::to_string(ESP_getFlashSizeKB());
		response += PSTR(" KB, ");
		response += ksf::to_string(ESP.getFlashChipSpeed()/1000000);
		response += PSTR(" MHz)\"},{\"name\":\"Hostname\",\"value\":\"");
		response += WiFi.getHostname();
		response += PSTR("\"},{\"name\":\"Free heap\",\"value\":\"");
		response += ksf::to_string(ESP.getFreeHeap());
		response += PSTR(" bytes\"},{\"name\":\"Device uptime\",\"value\":\"");
		response += ksf::getUptimeString();
		response += PSTR("\"},{\"name\":\"Reset reason\",\"value\":\"");
		response += ksf::getResetReason();
		response += PSTR("\"},{\"name\":\"IP address\",\"value\":\"");
		response += WiFi.getMode() == WIFI_AP ?  WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str();
		response += PSTR("\"}]");
	}

	void ksDevicePortal::handle_scanNetworks(std::string& response)
	{
		switch (WiFi.scanComplete())
		{
			case WIFI_SCAN_FAILED:
				WiFi.scanNetworks(true);
			case WIFI_SCAN_RUNNING:
				response += PSTR("{}");
			return;

			default:
			{
				response += '[';
				for (int i{0}; i < WiFi.scanComplete(); ++i)
				{
					if (i > 0)
						response += ',';
					response += PSTR("{\"rssi\":");
					response += ksf::to_string(WiFi.RSSI(i));
					response += PSTR(",\"ssid\":\"");
					response += WiFi.SSID(i).c_str();
					response += PSTR("\",\"bssid\":\"");
					response += WiFi.BSSIDstr(i).c_str();
					response += PSTR("\",\"channel\":");
					response += ksf::to_string(WiFi.channel(i));
					response += PSTR(",\"secure\":");
					response += ksf::to_string(WiFi.encryptionType(i));
					response += '}';
				}
				response +=	']';
				WiFi.scanDelete();
			
				WiFi.enableSTA(false);
			}
		}
	}

	void ksDevicePortal::handle_getDeviceParams(std::string& response)
	{
		std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
		owner->findComponents<ksConfigProvider>(configCompsWp);
		bool isInConfigMode{!configCompsWp.empty()};

		response += PSTR("{\"isConfigMode\": ");
		response += isInConfigMode ? PSTR("true") : PSTR("false");

		if (!isInConfigMode)
		{
			response += '}';
			return;
		}

		std::string ssid, pass;
		ksf::loadCredentials(ssid, pass);

		response += PSTR(",\"ssid\":\"");
		response += ssid;
		response += PSTR("\", \"password\":\"");
		response += pass;
		response += PSTR("\",\"params\": [");

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
				response += PSTR("{\"id\": \"");
				response += paramRef.id;
				response += PSTR("\", \"value\": \"");
				response += paramRef.value;
				response += PSTR("\", \"default\": \"");
				response += paramRef.defaultValue;
				response += PSTR("\"},");
			}
		}

		if (response.back() == ',')
			response.pop_back();
		
		response += "]}";
	}

	void ksDevicePortal::onRequest_notFound() const
	{
		/* For files always return 404. */
		if (webServer->header(PROGMEM_ACCEPT).indexOf(PROGMEM_TEXT_HTML) == -1)
		{
			webServer->send(404);
			return;
		}

		/* Redirect all html requests to the main page. */
		webServer->sendHeader(PSTR("Location"), PSTR("/"));
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
				return webServer->send_P(400, PROGMEM_TEXT_PLAIN, PSTR("OTA could not begin"));

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

		webServer->sendHeader(PSTR("Connection"), PSTR("close"));
		webServer->send(200, PROGMEM_TEXT_PLAIN, hasError ? PSTR("FAIL") : PSTR("OK"));

		if (hasError)
			return;

		updateFinished(true);
		rebootDevice();
	}
	
	void ksDevicePortal::onRequest_index() const
	{
		if (inRequest_NeedAuthentication())
			return;

		const auto& fileMD5{FPSTR(DEVICE_FRONTEND_HTML_MD5)};
		if (webServer->header(PROGMEM_IF_NONE_MATCH) == fileMD5)
		{
			webServer->send(304);
			return;
		}

		webServer->sendHeader(PSTR("ETag"), fileMD5);
		webServer->sendHeader(PSTR("Content-Encoding"), PSTR("gzip"));
		webServer->send_P(200, PROGMEM_TEXT_HTML, (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
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

		webServer->on("/api/flash", HTTP_POST, 
			std::bind(&ksDevicePortal::onRequest_otaFinish, this),	// Upload file part
			std::bind(&ksDevicePortal::onRequest_otaChunk, this)	// Upload file end
		);

		/* Setup headers we want to collect. */
		const char* headerkeys[] = 
		{
			PROGMEM_ACCEPT, 
			PROGMEM_IF_NONE_MATCH
		};

		webServer->collectHeaders(headerkeys, sizeof(headerkeys)/sizeof(char*));
		webServer->begin();

		webSocket = std::make_unique<WebSocketsServer>(81);

		webSocket->enableHeartbeat(2000, 5000, 2);

		// TODO: Add auth
		//webSocket->setAuthorization(PSTR("admin"), password.c_str());

		webSocket->onEvent([this](uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
			if (type == WStype_TEXT)
				this->onWebsocketTextMessage(num, std::string_view((const char*)payload, length));
		});

		webSocket->begin();
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

		if (webSocket)
			webSocket->loop();

		return true;
	}
}
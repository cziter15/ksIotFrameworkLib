/*
 *	Copyright (c) 2020-2025, Krzysztof Strehlau
 *
 *	This file is a part of the ksIotFrameworkLib IoT library.
 *	All licensing information can be found inside LICENSE.md file.
 *
 *	https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

#include <map>
#include <DNSServer.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#if defined(ESP32) || defined(ESP8266)
	#if defined(ESP8266)
		#define HARDWARE "ESP8266"
		#include "flash_hal.h"
		#include <ESP8266WiFi.h>
		#include <ESP8266WebServer.h>
		using WebServerClass = ESP8266WebServer;
		uint8_t ESP_getFlashVendor() { return ESP.getFlashChipVendorId(); }
		uint32_t ESP_getFlashSizeKB() { return ESP.getFlashChipRealSize()/1024; }
	#elif defined(ESP32)
		#define HARDWARE "ESP32"
		#include <WiFi.h>
		#include <WebServer.h>
		#include "nvs_flash.h"
		using WebServerClass = WebServer;
		extern uint32_t ESP_getFlashChipId(void);
		uint8_t ESP_getFlashVendor() { return ESP_getFlashChipId() & 0xFF; }
		uint32_t ESP_getFlashSizeKB() { return ESP.getFlashChipSize()/1024; }
	#endif
#else
	#error Platform not implemented.
#endif

#include "../ksApplication.h"
#include "../ksConstants.h"
#include "../misc/ksWSServer.h"
#include "../res/otaWebpage.h"
#include "ksWifiConnector.h"
#include "ksConfigProvider.h"
#include "ksMqttConnector.h"

#include "ksDevicePortal.h"

using namespace std::placeholders;

namespace ksf::comps
{
	constexpr auto WIFI_SCAN_TIMEOUT{15000UL};
	constexpr auto LOG_KEEPALIVE_INTERVAL{8000UL};

	constexpr auto WS_PING_INTERVAL_MS{2000UL};
	constexpr auto WS_PONG_TIMEOUT_MS{5000UL};
	constexpr auto WS_DISCONNECT_TIMEOUT_COUNT{2U};

	static constexpr char PROGMEM_TEXT_PLAIN[] PROGMEM {"text/plain"};
	static constexpr char PROGMEM_APPLICATION_JSON[] PROGMEM {"application/json"};
	static constexpr char PROGMEM_TEXT_HTML[] PROGMEM {"text/html"};
	static constexpr char PROGMEM_ACCEPT[] PROGMEM {"Accept"};
	static constexpr char PROGMEM_IF_NONE_MATCH[] PROGMEM {"If-None-Match"};
	static constexpr char PROGMEM_NO_ID_RESPONSE[] PROGMEM {"null\n"};

	inline uint64_t generateAuthToken(const std::string& password)
	{
#if defined(ESP32)
		uint64_t chipId{ESP.getEfuseMac()};
#elif defined(ESP8266)
		uint32_t chipId32{ESP.getChipId()};
		uint64_t chipId{(uint64_t)chipId32 << 32 | ~chipId32};
#else
		#error Platform not implemented.
#endif
		std::hash<std::string> hasher;
		return chipId ^ hasher(password);
	}

	ksDevicePortal::ksDevicePortal()
		: ksDevicePortal(PSTR("ota_ksiotframework"))
	{}

	ksDevicePortal::ksDevicePortal(std::string portalPassword)
		: portalPassword(std::move(portalPassword)), arduinoOTA(std::make_unique<ArduinoOTAClass>())
	{
		arduinoOTA->setPassword(this->portalPassword.c_str());
		
		arduinoOTA->onStart([this]() {
			onUpdateStart->broadcast();
		});

		arduinoOTA->onEnd([this]() {
			updateFinished(false);
		});
	}

	ksDevicePortal::~ksDevicePortal()
	{
		/*
			Watch out! It looks like ArduinoOTA library for both ESP8266 and ESP32 miss end() call in its destructor.
			We should manually call it here. Otherwise we may get exception (crash) during application transition.
		*/
		if (bitflags.isSafeToCallEndOta && arduinoOTA)
			arduinoOTA->end();
	}
	
	bool ksDevicePortal::init(ksApplication* app)
	{
		this->app = app;
		return true;
	}

	bool ksDevicePortal::postInit(ksApplication* app)
	{
		arduinoOTA->setHostname(WiFi.getHostname());
		arduinoOTA->begin();
		bitflags.isSafeToCallEndOta = true;

		if (WiFi.getMode() == WIFI_AP) 
		{
			dnsServer = std::make_unique<DNSServer>();
			dnsServer->setErrorReplyCode(DNSReplyCode::NoError);
			dnsServer->start(53, "*", WiFi.softAPIP());
		}
		
		mqttConnectorWp = app->findComponent<ksMqttConnector>();

		setupHttpServer();
		setupWsServer();

		return true;
	}

	void ksDevicePortal::onAppLog(std::string&& message)
	{
		if (!webSocket)
			return;

		message.insert(0, PROGMEM_NO_ID_RESPONSE);
		webSocket->broadcastTXT(message.c_str(), message.size());
	}

	void ksDevicePortal::rebootDevice()
	{
		delay(100);
		ESP.restart();
	}

	void ksDevicePortal::triggerFactoryReset()
	{
		WiFi.mode(WIFI_OFF);
		eraseConfigData();
		rebootDevice();
	}

	std::string ksDevicePortal::handle_executeCommand(const std::string_view& body)
	{
		if (body.empty())
			return PSTR("No command received. Don't be shy. Try 'help'.");
		if (body == PSTR("help"))
			return PSTR("Available commands: reboot-device, erase-config, erase-all-data, help");
		
		if (body == PSTR("reboot-device"))
			rebootDevice();
		else if (body == PSTR("erase-config"))
			triggerFactoryReset();
		else if (body == PSTR("erase-all-data"))
		{
			WiFi.mode(WIFI_OFF);
			#if ESP32
				nvs_flash_erase();
			#elif ESP8266
				ESP.eraseConfig();
			#endif
			LittleFS.format();
			rebootDevice();
		}
		else
		{
			bool handled{false};
			std::string response;
			onHandlePortalCommand->broadcast(body, handled, response);
			if (handled)
				return response;
		}

		return PSTR("[ DevicePortal ] Command not recognized: ") + std::string(body);
	}

	void ksDevicePortal::updateFinished(bool fromPortal)
	{
		ksf::saveOtaBootIndicator(fromPortal ? EOTAType::OTA_PORTAL : EOTAType::OTA_GENERIC);
		onUpdateEnd->broadcast();
	}

	bool ksDevicePortal::inRequest_NeedAuthentication()
	{
		if (WiFi.localIP() != webServer->client().localIP())
			return false;

		if (portalPassword.empty() || webServer->authenticate(PSTR("admin"), portalPassword.c_str()))
			return false;

		webServer->requestAuthentication();
		return true;
	}

	void ksDevicePortal::onWebsocketTextMessage(uint8_t clientNum, const std::string_view& message)
	{
		auto idEnd{message.find('|', 0)};
		if (idEnd == std::string_view::npos)
			return;

		auto commandStart{idEnd + 1};
		if (commandStart >= message.size())
			return;

		auto commandEnd{message.find('\n', commandStart + 1)};
		if (commandEnd == std::string_view::npos)
			commandEnd = message.size();

		auto id{message.substr(0, idEnd)};
		auto command{message.substr(commandStart, commandEnd - commandStart)};
		
		std::string_view body;
		if (commandEnd + 1 < message.size())
			body = message.substr(commandEnd + 1);

		if (command == PSTR("executeCommand"))
		{
			auto commandResponse{handle_executeCommand(body)};
			commandResponse.insert(0, PROGMEM_NO_ID_RESPONSE);
			webSocket->sendTXT(clientNum, commandResponse.c_str(), commandResponse.size());
			return;
		}

		std::string response{id};
		response += '\n';

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
		else if (command == PSTR("factoryReset"))
		{
			triggerFactoryReset();
			return;
		}
		else if (command == PSTR("saveConfig"))
		{
			/* Grab all config components. */
			std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
			app->findComponents<ksConfigProvider>(configCompsWp);

			/* Parse body. */
			std::map<std::string_view, std::string_view> paramMap;
			size_t startPos{0}, endPos{body.find('\n')};
			while (endPos != std::string_view::npos) 
			{
				auto line{body.substr(startPos, endPos - startPos)};
				size_t equalPos{line.find('=')};

				if (equalPos != std::string_view::npos) 
					paramMap[line.substr(0, equalPos)] = line.substr(equalPos + 1);

				startPos = endPos + 1;
				endPos = body.find('\n', startPos);
			}

			/* Save WiFi credentials. */
			std::string ssid{paramMap[PSTR("ssid")]};
			std::string password{paramMap[PSTR("password")]};
			ksf::saveCredentials(ssid, password);

			/* Save user parameters. */
			for (auto& configCompWp : configCompsWp)
			{
				auto configCompSp{configCompWp.lock()};
				if (!configCompSp)
					continue;

				for (auto& parameter : configCompSp->getParameters())
				{
					auto param_id{PSTR("param_") + parameter.id};
					if (auto param_it{paramMap.find(param_id)}; param_it != paramMap.end())
					{
						auto& [paramName, paramValue] = *param_it;
						parameter.value = paramValue;
					}
				}

				configCompSp->saveParams();
			}

			requestAppBreak();
			return;
		}
		else if (command == PSTR("logKeepAlive"))
		{
			auto enabledLogsRightNow{ logKeepAliveTimestamp == 0 };
			logKeepAliveTimestamp = std::max(1UL, millis());

			if (enabledLogsRightNow)
			{
#if APP_LOG_ENABLED
				app->setLogCallback(std::bind(&ksDevicePortal::onAppLog, this, _1));
				std::string log{PSTR("[ DevicePortal ] Logs enabled - both command responses and detailed logs will be printed.")};
#else
				std::string log{PSTR("[ DevicePortal ] Detailed logs are disabled (no APP_LOG_ENABLED set). Only command responses will be printed.")};
#endif
				onAppLog(std::move(log));
			}

			return;
		}

		webSocket->sendTXT(clientNum, response.c_str(), response.size());
	}

	void ksDevicePortal::handle_getIdentity(std::string& response)
	{
		response += PSTR("[{\"name\":\"MCU chip\",\"value\":\"");
		response += PSTR(HARDWARE " (");
		response += ksf::to_string(ESP.getCpuFreqMHz());
		response += PSTR(" MHz)\"},{\"name\":\"Flash chip\",\"value\":\"Vendor ID: ");
		response += ksf::to_string(ESP_getFlashVendor());
		response += PSTR(", size: ");
		response += ksf::to_string(ESP_getFlashSizeKB());
		response += PSTR(" KB \"},{\"name\":\"Hostname\",\"value\":\"");
		response += WiFi.getHostname();
		response += PSTR("\"},{\"name\":\"Free heap\",\"value\":\"");
		response += ksf::to_string(ESP.getFreeHeap());
		response += PSTR(" bytes\"},{\"name\":\"Loop-to-loop time\",\"value\":\"");
		response += ksf::to_string(loopExecutionTime);
		response += PSTR(" μs\"},{\"name\":\"Device uptime\",\"value\":\"");
		response += ksf::getUptimeString();
		response += PSTR("\"},{\"name\":\"Reset reason\",\"value\":\"");
		response += ksf::getResetReason();
		response += PSTR("\"},{\"name\":\"MQTT status\",\"value\":\" ");

		if (auto mqttConnSp{mqttConnectorWp.lock()})
		{
			if (mqttConnSp->isConnected())
			{
				response += PSTR("up for ");
				response += ksf::getUptimeFromSeconds(mqttConnSp->getConnectionTimeSeconds());
				response += PSTR(", ");
			}
			else response += PSTR("down, ");

			response += std::to_string(mqttConnSp->getReconnectCounter());
			response += PSTR(" attempt(s)");
		}
		else response += PSTR("not present");

		response += PSTR("\"},{\"name\":\"IP address\",\"value\":\"");
		response += WiFi.getMode() == WIFI_AP ?  WiFi.softAPIP().toString().c_str() : WiFi.localIP().toString().c_str();
		response += PSTR("\"},{\"name\":\"DNS servers\",\"value\":\"");
		response += WiFi.dnsIP().toString().c_str();
		response += ", ";
		response += WiFi.dnsIP(1).toString().c_str();
		response += PSTR("\"}]");
	}

	void ksDevicePortal::handle_scanNetworks(std::string& response)
	{
		switch (WiFi.scanComplete())
		{
			case WIFI_SCAN_FAILED:
			{
#if defined(ESP32)
				WiFi.scanNetworks(true, false, true);
#elif defined(ESP8266)
				WiFi.scanNetworks(true, true);
#else
				#error Platform not implemented.
#endif
				scanNetworkTimestamp = std::max(1UL, millis());
			}
			case WIFI_SCAN_RUNNING:
			{
				response += PSTR("{}");
			}
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
					response += PSTR("\",\"channel\":");
					response += ksf::to_string(WiFi.channel(i));
					response += PSTR(",\"secure\":");
					response += ksf::to_string(WiFi.encryptionType(i));
					response += '}';
				}
				response +=	']';

				WiFi.scanDelete();
				WiFi.enableSTA(false);
				scanNetworkTimestamp = 0;
			}
		}
	}

	void ksDevicePortal::handle_getDeviceParams(std::string& response)
	{
		std::vector<std::weak_ptr<ksConfigProvider>> configCompsWp;
		app->findComponents<ksConfigProvider>(configCompsWp);

		auto isInConfigMode{static_cast<bool>(WiFi.getMode() & WIFI_AP)};
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
				response += PSTR("\", \"label\": \"");
				response += paramRef.label;
				response += PSTR("\", \"value\": \"");
				response += paramRef.value;
				response += PSTR("\", \"type\": \"");
				
				switch (paramRef.type)
				{
					case EConfigParamType::Text:
						response += PSTR("text");
					break;
					case EConfigParamType::Password:
						response += PSTR("password");
					break;
					case EConfigParamType::Number:
						response += PSTR("number");
					break;
					case EConfigParamType::Checkbox:
						response += PSTR("checkbox");
					break;
				}

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
	
		auto& upload{webServer->upload()};
		if (upload.status == UPLOAD_FILE_START)
		{
#if defined(ESP8266)
			Update.runAsync(true);
			auto maxSketchSpace{(ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000};
#elif defined(ESP32)
			auto maxSketchSpace{UPDATE_SIZE_UNKNOWN};
#else
			#error Platform not implemented.
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

		auto hasError{Update.hasError()};

		webServer->sendHeader(PSTR("Connection"), PSTR("close"));
		webServer->send(200, PROGMEM_TEXT_PLAIN, hasError ? PSTR("FAIL") : PSTR("OK"));

		if (hasError)
			return;

		updateFinished(true);
		rebootDevice();
	}
	
	void ksDevicePortal::onRequest_index()
	{
		if (inRequest_NeedAuthentication())
			return;

		if (webSocket)
		{
			String cookie{PSTR("WSA=")};
			cookie += webSocket->getRequiredAuthToken();
			cookie += PSTR("; Path=/; HttpOnly; SameSite=Strict");
			webServer->sendHeader(PSTR("Set-Cookie") , cookie);
		}

		const auto& fileMD5{FPSTR(DEVICE_FRONTEND_HTML_MD5)};
		if (webServer->header(PROGMEM_IF_NONE_MATCH) == fileMD5)
		{
			webServer->send(304);
			return;
		}

		webServer->sendHeader(PSTR("Content-Encoding"), PSTR("gzip"));
		webServer->sendHeader(PSTR("ETag"), fileMD5);
		webServer->send_P(200, PROGMEM_TEXT_HTML, (const char*)DEVICE_FRONTEND_HTML, DEVICE_FRONTEND_HTML_SIZE);
	}

	void ksDevicePortal::setupHttpServer()
	{
		webServer = std::make_unique<WebServerClass>(80);
		
		/* Setup 404 handler. */
		webServer->onNotFound(std::bind(&ksDevicePortal::onRequest_notFound, this));

		/* Setup index page handler. */
		webServer->on(FPSTR("/"), HTTP_GET,
			std::bind(&ksDevicePortal::onRequest_index, this)
		);

		/* Setup OTA update request handler. */
		webServer->on("/api/flash", HTTP_POST,
			std::bind(&ksDevicePortal::onRequest_otaFinish, this),	// Upload file part
			std::bind(&ksDevicePortal::onRequest_otaChunk, this)	// Upload file end
		);

		/* Setup headers we want to collect. */
		const char* headerkeys[]
		{
			PROGMEM_ACCEPT, 
			PROGMEM_IF_NONE_MATCH
		};
		webServer->collectHeaders(headerkeys, sizeof(headerkeys)/sizeof(char*));

		/* Startup. */
		webServer->begin();
	}

	void ksDevicePortal::setupWsServer()
	{
		webSocket = std::make_unique<ksf::misc::ksWSServer>(81);

		auto authTokenToHash{portalPassword};
		authTokenToHash.append(DEVICE_FRONTEND_HTML_MD5);
		webSocket->setRequiredAuthToken(generateAuthToken(authTokenToHash));

		webSocket->setMessageHandler(std::bind(&ksDevicePortal::onWebsocketTextMessage, this, _1, _2));
		webSocket->enableHeartbeat(WS_PING_INTERVAL_MS, WS_PONG_TIMEOUT_MS, WS_DISCONNECT_TIMEOUT_COUNT);

		webSocket->begin();
	}

	bool ksDevicePortal::loop(ksApplication* app)
	{
		loopExecutionTime = micros() - lastLoopExecutionTimestamp;

		/* Return from the app on request. */
		if (bitflags.breakApp)
			return false;

		/* Handle OTA stuff. */
		if (arduinoOTA)
			arduinoOTA->handle();

		/* Handle requests. */
		if (dnsServer)
			dnsServer->processNextRequest();

		/* Handle web server. */
		if (webServer)
			webServer->handleClient();

		/* Handle WebSocket. */
		if (webSocket)
			webSocket->loop();

		/* Cleanup scan results if not received by the client. */
		if (scanNetworkTimestamp != 0 && millis() - scanNetworkTimestamp > WIFI_SCAN_TIMEOUT)
		{
			scanNetworkTimestamp = 0;
			WiFi.scanDelete();
			WiFi.enableSTA(false);
		}

		if (logKeepAliveTimestamp != 0 && millis() - logKeepAliveTimestamp > LOG_KEEPALIVE_INTERVAL)
		{
#if APP_LOG_ENABLED
			app->setLogCallback(nullptr);
#endif
			logKeepAliveTimestamp = 0;
		}

		lastLoopExecutionTimestamp = micros();
		return true;
	}
}

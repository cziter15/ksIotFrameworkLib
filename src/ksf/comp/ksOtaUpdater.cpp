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
#include "ksOtaUpdater.h"

#if USE_ELEGANT_OTA
#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#elif defined(ESP32)
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#endif
#include <ElegantOTA.h>
#if defined(ESP8266)
ESP8266WebServer server(80);
#elif defined(ESP32)
WebServer server(80);
#endif
#endif
namespace ksf::comps
{
	ksOtaUpdater::ksOtaUpdater()
		: ksOtaUpdater(PGM_("ota_ksiotframework"))
	{}

	ksOtaUpdater::ksOtaUpdater(const std::string& password)
	{
		ArduinoOTA.setPassword(password.c_str());

#if USE_ELEGANT_OTA
		ElegantOTA.begin(&server, "admin", password.c_str());
#endif

		ArduinoOTA.onStart([this]() {
			onUpdateStart->broadcast();
		});

		ArduinoOTA.onEnd([this]() {
			this->updateFinished();
		});
	}

	void ksOtaUpdater::updateFinished()
	{
		ksf::saveOtaBootIndicator();
		onUpdateEnd->broadcast();
	}
	
	bool ksOtaUpdater::init(ksApplication* owner)
	{
		ArduinoOTA.setHostname(WiFi.getHostname());
		ArduinoOTA.begin();

#if USE_ELEGANT_OTA
		ElegantOTA.setID(WiFi.getHostname());
		server.begin();

		static ksOtaUpdater * thisPtr{this};
		ElegantOTA.onOTAEnd([]() {
			thisPtr->updateFinished();
		});

		server.on("/", [this]() {
			server.send(200, "text/html", getRootContent().c_str());
		});
#endif

		return true;
	}

	const std::string ksOtaUpdater::getRootContent() const
	{
		std::string html = "<html><body>";
		html += "<h1>" + std::string(WiFi.getHostname()) + "</h1>";
		html += "<h2>Device Info</h2>";
		html += "<ul>";
		html += "<li>Chip ID: " + std::to_string(ESP.getChipId()) + "</li>";
		html += "<li>Flash Chip ID: " + std::to_string(ESP.getFlashChipId()) + "</li>";
		html += "<li>Flash Chip Size: " + std::to_string(ESP.getFlashChipSize()) + " bytes</li>";
		html += "<li>Free Heap: " + std::to_string(ESP.getFreeHeap()) + " bytes</li>";
		html += "</ul>";
		html += "<h2>Update</h2>";
		html += "<a href='/update'>Go to OTA update page</a>";
		html += "</body></html>";
		return html;
	}

	bool ksOtaUpdater::loop()
	{
		/* Handle OTA stuff. */
		ArduinoOTA.handle();

#if USE_ELEGANT_OTA
		/* Handle HTTP stuff. */
		server.handleClient();
#endif

		return true;
	}
}
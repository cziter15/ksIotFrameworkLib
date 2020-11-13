/*
	 Project:	ksIotFramework
	 Author:	Krzysztof Strehlau
*/

#pragma once

#include <WString.h>
#include <map>

#define USING_CONFIG_FILE(fileName) if (ksf::ksConfig config_file = ksf::ksConfig(fileName))

namespace ksf
{
	class ksConfig
	{
		protected:
			bool isDirty = false;
			std::map<String, String> configParams;
			String configFilename;

		public:
			ksConfig(String configFile);
			void setParam(const String& paramName, String paramValue);
			const String& getParam(const String& paramName, const String& defaultValue = String()) const;
			virtual ~ksConfig();

			operator bool() const;
	};
}


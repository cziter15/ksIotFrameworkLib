/*
 *  Copyright (c) 2020-2026, Krzysztof Strehlau
 *
 *  This file is a part of the ksIotFrameworkLib IoT library.
 *  All licensing information can be found inside LICENSE.md file.
 *
 *  https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE
 */

 #include <LittleFS.h>
 #include <array>
 #include <string_view>
 
 #include "../ksConstants.h"
 
 #include "ksConfig.h"

 namespace ksf::misc
 {
	ksConfig::ksConfig(const std::string& fileName)
		: isDirty{false}, configPath{getNvsDirectory()}
	{
		/* If no file specified, clear internal path. */
		if (fileName.empty())
		{
			configPath.clear();
			return;
		}

		/* Add leading slash and assemble file path. */
		if (fileName[0] != '/')
			configPath += '/';
		configPath += fileName;

		/* Read config file. */
		File fileReader{LittleFS.open(configPath.c_str(), "r")};
		if (fileReader)
		{
			/* Use std::array for fixed buffer with automatic memory management */
			constexpr size_t bufferSize{256};
			std::array<char, bufferSize> buffer{};
			
			std::string key;
			bool isReadingKey{true};
			
			while (fileReader.available())
			{
				/* Ensure we leave space for null terminator */
				size_t maxReadSize{buffer.size() - 1};
				size_t bytesRead{fileReader.readBytesUntil('\n', buffer.data(), maxReadSize)};
				
				/* Safety check to prevent buffer overrun */
				if (bytesRead >= maxReadSize)
					bytesRead = maxReadSize;
				
				/* Ensure null termination */
				buffer[bytesRead] = '\0';
				
				/* Remove trailing CR if present */
				if (bytesRead > 0 && buffer[bytesRead - 1] == '\r')
					buffer[bytesRead - 1] = '\0';
				
				/* Use string_view to avoid unnecessary copying */
				std::string_view line{buffer.data()};
				
				if (isReadingKey)
				{
					key = line;
					isReadingKey = false;
				}
				else
				{
					configParams[key] = std::string{line};
					isReadingKey = true;
				}
			}
			fileReader.close();
		}
	}
	
	ksConfig::~ksConfig()
	{
		if (!isDirty || configPath.empty())
			return;
	
		/* If marked dirty, save changes to FS. */
		File fileWriter{LittleFS.open(configPath.c_str(), "w")};
		if (!fileWriter)
			return;
	
		for (const auto& [name, value] : configParams)
		{
			fileWriter.println(name.c_str());
			fileWriter.println(value.c_str());
		}
		fileWriter.close();
	}
	void ksConfig::setParam(const std::string& paramName, std::string paramValue)
	{
		isDirty = true;
		configParams[paramName] = std::move(paramValue);
	}
	
	const std::string& ksConfig::getParam(const std::string& paramName, const std::string& defaultValue) const
	{
		auto it{configParams.find(paramName)};
		if (it != configParams.end())
			return it->second;
		
		if (!defaultValue.empty())
			return defaultValue;
		
		static const std::string emptyString;
		return emptyString;
	}
	
	ksConfig::operator bool() const
	{
		return !configPath.empty();
	}
}

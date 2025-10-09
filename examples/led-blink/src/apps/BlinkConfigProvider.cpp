#include "BlinkConfigProvider.h"

namespace apps
{
	void BlinkConfigProvider::readParams()
	{
		// Open the configuration file for reading
		USING_CONFIG_FILE(CONFIG_FILENAME)
		{
			// Add a parameter for the blink interval
			// - id: "blink_interval" - used internally and as the config key
			// - label: "LED Blink Interval (ms)" - shown in the configuration UI
			// - config_file: reference to read current value from storage
			// - maxLength: 10 - allows values up to 10 digits (9,999,999,999 ms)
			// - type: Number - tells the UI to render a number input field
			addNewParamWithConfigDefault(config_file, BLINK_INTERVAL_PARAM, "LED Blink Interval (ms)", 10, ksf::comps::EConfigParamType::Number);
		}
	}

	void BlinkConfigProvider::saveParams()
	{
		// Open the configuration file for writing
		USING_CONFIG_FILE(CONFIG_FILENAME)
		{
			// Iterate through all parameters and save them to the config file
			// Use std::move to efficiently transfer the string values
			for (auto& param : params)
				config_file.setParam(param.id, std::move(param.value));
		}
		
		// Clear the parameters list after saving
		params.clear();
	}
}

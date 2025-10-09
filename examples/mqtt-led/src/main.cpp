/**
 * MQTT LED Control Example for ksIotFrameworkLib
 * 
 * This example demonstrates how to create an IoT device that controls an LED
 * via MQTT messages. The LED can be turned on/off remotely through MQTT topics.
 * 
 * FEATURES:
 * - WiFi connectivity with automatic reconnection
 * - MQTT connection with configurable broker settings
 * - Remote LED control via MQTT messages
 * - Web-based configuration interface (AP mode)
 * - Application rotation (switches between MqttLedApp and ConfigApp)
 * 
 * HOW IT WORKS:
 * The device uses an application rotator that cycles between two applications:
 * 
 * 1. MqttLedApp (Primary Mode):
 *    - Connects to WiFi using saved credentials
 *    - Connects to MQTT broker using saved configuration
 *    - Subscribes to "led" topic to receive control commands
 *    - Accepts "on" or "off" messages to control the LED
 *    - If WiFi or MQTT connection fails, rotates to ConfigApp
 * 
 * 2. ConfigApp (Configuration Mode):
 *    - Creates an Access Point named "MqttLed-XXXX"
 *    - Provides a web interface to configure:
 *      * WiFi credentials (SSID and password)
 *      * MQTT broker settings (address, port, credentials, prefix)
 *    - After configuration is saved, device reboots to MqttLedApp
 * 
 * HARDWARE SETUP:
 * - Connect an LED to the pin defined in board.h (default: GPIO 2)
 * - The LED should have a current-limiting resistor (typically 220-330Ω)
 * - For built-in LEDs, no additional hardware is needed
 * 
 * FIRST-TIME SETUP:
 * 1. Flash the firmware to your ESP device
 * 2. The device will start in ConfigApp mode (no configuration saved)
 * 3. Connect to "MqttLed-XXXX" WiFi network (XXXX = device UUID)
 * 4. Open a browser and navigate to 192.168.4.1
 * 5. Configure WiFi credentials and MQTT broker settings
 * 6. Save settings - device will reboot and connect to MQTT
 * 
 * MQTT CONTROL:
 * - Subscribe topic: <prefix>/led (where prefix is configured in ConfigApp)
 * - Send "on" to turn LED on
 * - Send "off" to turn LED off
 * 
 * CHANGING CONFIGURATION:
 * - If the device cannot connect to WiFi or MQTT, it automatically enters ConfigApp mode
 */

#include "apps/ConfigApp.h"
#include "apps/MqttLedApp.h"

using namespace apps;

// Application rotator macro - defines the order of applications
// The framework will try each application in sequence:
// - First attempts MqttLedApp (normal operation)
// - If MqttLedApp fails (e.g., WiFi/MQTT connection issue), tries ConfigApp
// This provides automatic fallback to configuration mode when needed
KSF_IMPLEMENT_APP_ROTATOR
(
	MqttLedApp,
	ConfigApp
)

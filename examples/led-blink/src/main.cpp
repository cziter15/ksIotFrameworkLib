/**
 * LED Blink Example for ksIotFrameworkLib
 * 
 * This example demonstrates how to create a simple IoT device that blinks an LED
 * at a configurable interval. The blink interval can be set through a web-based
 * configuration interface.
 * 
 * FEATURES:
 * - Configurable LED blink interval (stored in non-volatile memory)
 * - WiFi connectivity with automatic reconnection
 * - Web-based configuration interface (AP mode)
 * - Application rotation (switches between BlinkApp and ConfigApp)
 * 
 * HOW IT WORKS:
 * The device uses an application rotator that cycles between two applications:
 * 
 * 1. BlinkApp (Primary Mode):
 *    - Connects to WiFi using saved credentials
 *    - Reads the blink interval from configuration
 *    - Blinks the LED at the configured interval
 *    - If WiFi connection fails, rotates to ConfigApp
 * 
 * 2. ConfigApp (Configuration Mode):
 *    - Creates an Access Point named "LedBlinkDevice"
 *    - Provides a web interface to configure:
 *      * WiFi credentials (SSID and password)
 *      * LED blink interval
 *    - After configuration is saved, device reboots to BlinkApp
 * 
 * HARDWARE SETUP:
 * - Connect an LED to the pin defined in board.h (default: GPIO 2)
 * - The LED should have a current-limiting resistor (typically 220-330Ω)
 * - For built-in LEDs, no additional hardware is needed
 * 
 * FIRST-TIME SETUP:
 * 1. Flash the firmware to your ESP device
 * 2. The device will start in ConfigApp mode (no WiFi credentials saved)
 * 3. Connect to "LedBlinkDevice" WiFi network
 * 4. Open a browser and navigate to the device's IP (usually 192.168.4.1)
 * 5. Configure WiFi credentials and LED blink interval
 * 6. Save settings - device will reboot and start blinking
 * 
 * CHANGING CONFIGURATION:
 * - If the device cannot connect to WiFi, it automatically enters ConfigApp mode
 * - You can also trigger ConfigApp by erasing configuration or using OTA update
 */

#include "apps/ConfigApp.h"
#include "apps/BlinkApp.h"

using namespace apps;

// Application rotator macro - defines the order of applications
// The framework will try each application in sequence:
// - First attempts BlinkApp (normal operation)
// - If BlinkApp fails (e.g., WiFi connection issue), tries ConfigApp
// This provides automatic fallback to configuration mode when needed
KSF_IMPLEMENT_APP_ROTATOR
(
	BlinkApp,
	ConfigApp
)

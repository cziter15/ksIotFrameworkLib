# MQTT LED Control Example

This example demonstrates how to control an LED remotely using MQTT messages with the ksIotFrameworkLib IoT library.

## 📋 Overview

The MQTT LED example shows how to build a simple IoT device that can be controlled remotely via MQTT. The device connects to a WiFi network and an MQTT broker, then listens for commands to turn an LED on or off. This is a foundational pattern for building more complex IoT devices that respond to cloud commands.

## ✨ Features

- **Remote LED Control**: Turn the LED on/off via MQTT messages
- **WiFi Connectivity**: Automatic connection and reconnection
- **MQTT Integration**: Subscribes to control topics and responds to commands
- **Web Configuration**: Easy setup through Access Point mode
- **Persistent Settings**: WiFi and MQTT credentials stored in non-volatile memory
- **Automatic Fallback**: Switches to configuration mode if connection fails

## 🔧 Hardware Setup

### Requirements

- ESP8266 or ESP32 development board
- LED with current-limiting resistor (220-330Ω)
- USB cable for programming

### Wiring

```text
ESP Board          LED
GPIO 2 ------------|>|---[220Ω]--- GND
(or pin in board.h)
```

**Note**: Many ESP development boards have a built-in LED on GPIO 2, so no external LED may be needed.

## 🚀 Getting Started

### 1. Build and Flash

```bash
cd examples/mqtt-led
pio run -e esp8266dev --target upload  # For ESP8266
# or
pio run -e esp32 --target upload       # For ESP32
```

### 2. Initial Configuration

On first boot, the device has no saved configuration and will automatically enter configuration mode:

1. **Connect to WiFi**: Look for a network named `MqttLed-XXXX` (where XXXX is a unique device identifier)
2. **Open Configuration Page**: Navigate to `http://192.168.4.1` in your web browser
3. **Configure Settings**:
   - **WiFi SSID**: Your WiFi network name
   - **WiFi Password**: Your WiFi password
   - **MQTT Address**: Your MQTT broker address (e.g., `mqtt.example.com` or `192.168.1.100`)
   - **MQTT Port**: MQTT broker port (default: 1883)
   - **MQTT Username**: (Optional) MQTT username if required
   - **MQTT Password**: (Optional) MQTT password if required
   - **MQTT Topic Prefix**: Prefix for MQTT topics (e.g., `home/led1`)
4. **Save Configuration**: Click save - the device will reboot and connect

### 3. Control the LED

Once configured, the device will:

- Connect to your WiFi network
- Connect to the MQTT broker
- Subscribe to the topic: `<prefix>/led`

**To control the LED**, publish messages to this topic:

```bash
# Turn LED on
mosquitto_pub -h mqtt.example.com -t "home/led1/led" -m "on"

# Turn LED off
mosquitto_pub -h mqtt.example.com -t "home/led1/led" -m "off"
```

Replace `home/led1` with your configured MQTT prefix.

## ⚙️ How It Works

### Application Architecture

The firmware uses an **application rotator** pattern with two applications:

#### 1. MqttLedApp (Primary Mode)

- Connects to WiFi using saved credentials
- Connects to MQTT broker using saved settings
- Subscribes to `<prefix>/led` topic
- Listens for "on" and "off" commands
- Controls the LED based on received messages
- If connection fails, rotates to ConfigApp

#### 2. ConfigApp (Configuration Mode)

- Creates a WiFi Access Point named "MqttLed-XXXX"
- Hosts a web server at `192.168.4.1`
- Provides a configuration interface for:
  - WiFi SSID and password
  - MQTT broker address, port, and credentials
  - MQTT topic prefix
- Saves configuration to non-volatile storage
- Reboots device after saving

### Component Interaction

```text
MqttLedApp
  ├── ksWifiConnector (handles WiFi connection)
  ├── ksMqttConnector (handles MQTT connection)
  │   ├── onConnected event → subscribes to "led" topic
  │   └── onDeviceMessage event → controls LED
  └── ksLed (controls physical LED)
```

### MQTT Message Flow

1. Device connects to MQTT broker
2. Device subscribes to `<prefix>/led` topic
3. User publishes message to topic:
   - `"on"` → LED turns on
   - `"off"` → LED turns off
4. Device processes message in `onMqttMessage()` callback
5. LED state is updated via `ksLed` component

## 📁 Code Structure

```text
mqtt-led/
├── platformio.ini              # PlatformIO configuration
├── README.md                   # This file
└── src/
    ├── board.h                 # Hardware pin definitions
    ├── main.cpp                # Entry point with app rotator
    └── apps/
        ├── MqttLedApp.h        # Main application header
        ├── MqttLedApp.cpp      # Main application implementation
        ├── ConfigApp.h         # Configuration app header
        └── ConfigApp.cpp       # Configuration app implementation
```

### Key Files Explained

#### `board.h`

Defines the hardware pin configuration:

```cpp
#define LED_PIN 2  // Change this to match your LED pin
```

#### `main.cpp`

Sets up the application rotator:

- Tries `MqttLedApp` first (normal operation)
- Falls back to `ConfigApp` if connection fails

#### `MqttLedApp` (Main Application)

The primary application that:

- Manages WiFi and MQTT connections
- Subscribes to control topics
- Handles incoming MQTT messages
- Controls the LED hardware

Key methods:

- `init()`: Sets up components and registers callbacks
- `onMqttConnected()`: Subscribes to topics when MQTT connects
- `onMqttMessage()`: Processes incoming MQTT commands
- `loop()`: Main execution loop (delegated to framework)

#### `ConfigApp` (Configuration Application)

The configuration interface that:

- Creates an Access Point
- Hosts a web configuration page
- Saves settings to persistent storage
- Uses the framework's built-in `ksMqttConfigProvider`

## 🔧 Configuration

### MQTT Topic Structure

The device uses a prefix-based topic structure:

```text
<configured_prefix>/led
```

For example, if you set the prefix to `home/livingroom`, the LED control topic will be:

```text
home/livingroom/led
```

### Accepted Commands

Only two commands are recognized:

- `"on"` - Turn LED on (case-sensitive)
- `"off"` - Turn LED off (case-sensitive)

Any other messages are ignored.

## 🎓 Learning Objectives

This example teaches:

- How to integrate MQTT into your IoT application
- How to subscribe to MQTT topics and handle messages
- How to control hardware based on remote commands
- How to use the ksMqttConnector component
- How to implement event-driven architecture with callbacks
- How to structure an application with configuration mode

## 📚 Related Examples

- **led-blink**: Basic LED control without MQTT
- **basic-config**: More complex MQTT application with additional features

## 📄 License

This example is part of the ksIotFrameworkLib library.
See the main repository LICENSE file for licensing information.

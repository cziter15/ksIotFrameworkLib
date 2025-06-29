# ksIotFrameworkLib

> Arduino Library for ESP32/ESP8266 - a composition-oriented Internet of Things framework that provides a simple and extendable architecture, handles device setup (WiFi setup, MQTT and application-specific configuration), network connectivity, MQTT telemetry protocol, and more...

[![Read the documentation](https://img.shields.io/badge/Doxygen-2C4AA8?logo=doxygen&style=for-the-badge)](https://cziter15.github.io/ksIotFrameworkLib)
[![Read the wiki](https://img.shields.io/badge/wiki-258ffa?logo=wikidotjs&style=for-the-badge)](https://github.com/cziter15/ksIotFrameworkLib/wiki)
[![License](https://img.shields.io/github/license/cziter15/ksIotFrameworkLib?style=for-the-badge&color=44aa33)](https://github.com/cziter15/ksIotFrameworkLib/blob/master/LICENSE)

[![Codacy Badge](https://app.codacy.com/project/badge/Grade/956910bb43464108883bdcf57b1f6943)](https://app.codacy.com/gh/cziter15/ksIotFrameworkLib/dashboard?utm_source=gh&utm_medium=referral&utm_content=&utm_campaign=Badge_grade) 
[![Lines of Code](https://img.shields.io/endpoint?color=blue&url=https%3A%2F%2Fghloc.vercel.app%2Fapi%2Fcziter15%2Fksiotframeworklib%2Fbadge%3Ffilter%3D.hpp%24%2C.cpp%24%2C.h%24%26label%3DLines%2520of%2520Code)](https://github.com/cziter15/ksiotframeworklib)
[![Commit activity](https://img.shields.io/github/commit-activity/m/cziter15/ksIotFrameworkLib)](https://github.com/cziter15/ksIotFrameworkLib/commits/master)
[![Powered by platformio](https://img.shields.io/badge/powered%20by-platformio-orange?logo=platformio)](https://platformio.org)
[![Powered by pioarduino](https://img.shields.io/badge/powered%20by-pioarduino-orange?logo=arduino&color=darkgreen)](https://github.com/pioarduino)

![image](https://github.com/user-attachments/assets/d23e7245-659c-4639-a86e-5ac5bef78a43)

> **IMPORTANT FOR ESP32**
>
> This library targets Arduino 3+ on ESP32. Due to [PlatformIO limitations](https://github.com/platformio/platform-espressif32/issues/1225), it does not automatically fetch the latest versions.
> Use the pioarduino fork by Jason2866 in your `platformio.ini` file:
> ```ini
> platform = https://github.com/pioarduino/platform-espressif32/releases/download/stable/platform-espressif32.zip
> ```
>
> **IMPORTANT FOR ESP8266**
>
> For ESP8266, the latest supported version is based on SDK305.
> To use it, please add this build flag:
> 
> ```
> DPIO_FRAMEWORK_ARDUINO_ESPRESSIF_SDK305
> ```

## 📜 Motivation

- The goal of this project is to create a simple template or starting point for development of applications targeting Espressif microcontrollers.
- This project aims to streamline the process of copying and modifying source code for different devices.
- I wanted to apply DRY pattern for my DYI devices, by integrating all common components into a simple library.

## 📚 Documentation

- Detailed documentation can be found [here](https://cziter15.github.io/ksIotFrameworkLib).

## Examples

- For examples, refer to the [examples directory](https://github.com/cziter15/ksIotFrameworkLib/tree/master/examples).

## 🛠️ Architecture

```mermaid
flowchart TD
 subgraph Application_Init["Application_Init"]
        B("Mark app state as initialized")
        A("Add initial components")
  end
 subgraph Application_Loop["Application_Loop"]
        CCS{"State?"}
        Loop{{"For each component"}}
        LP1@{ label: "Call component's loop" }
        LP2@{ label: "Call component's init" }
        LP3@{ label: "Call component's postInit" }
        LP4("Remove component")
        DF{"Success?"}
        SCS2("State -> Initialized")
        SCS3("State -> Active")
        X0{{"Continue"}}
        X1{{"Break"}}
        Continue["Continue"]
  end
    AppState{"AppState"} -- NotInitialized --> Application_Init
    AppState -- Initialized --> Application_Loop
    A --> B
    Loop --> CCS
    CCS -- Active --> LP1
    CCS -- NotInitialized --> LP2
    CCS -- Initialized --> LP3
    CCS -- ToRemove --> LP4
    LP2 --> SCS2
    SCS2 --> DF
    LP3 --> SCS3
    SCS3 --> DF
    LP1 --> DF
    DF -- True --> X0
    DF -- False --> X1
    LP4 --> Continue
    LP1@{ shape: rounded}
    LP2@{ shape: rounded}
    LP3@{ shape: rounded}

```

- Only one application runs at a time.
- Each application manages its own set of components, the framework's core building blocks.
- Component states are managed within the application's `loop` function.
- Components implement `init`, `postInit`, and `loop` methods.
- Components marked for removal are safely deleted in the next cycle.

## 📏 Utilities and components structure

```text
    📁 ksf
    ├── 📄 ksAppRotator                 ─── Application rotator component
    ├── 📄 ksRtti                       ─── Implements RTTI (run-time type information) for objects
    ├── 📄 ksComponent                  ─── Base component class
    ├── 📄 ksConstants                  ─── Basic low-level definitions
    ├── 📂 misc
    │   ├── 📄 ksCertUtils              ─── MQTT certificate utilities
    │   ├── 📄 ksConfig                 ─── Configuration file handling
    │   ├── 📄 ksDomainQuery            ─── Custom DNS implementation
    │   ├── 📄 ksSimpleTimer            ─── Simple timer functionality
    │   └── 📄 ksWSServer               ─── Internal WS handling for device portal
    └── 📂 comps
        ├── 📄 ksConfigProvider         ─── Manages configuration parameters and storage
        ├── 📄 ksDevStatMqttReporter    ─── Sends periodic device status updates via MQTT
        ├── 📄 ksDevicePortal           ─── Implements a web-based configuration portal
        ├── 📄 ksLed                    ─── Simplifies LED control
        ├── 📄 ksMqttConfigProvider     ─── Manages MQTT-related configuration
        ├── 📄 ksMqttConnector          ─── Handles MQTT connection management
        ├── 📄 ksResetButton            ─── Implements reset button functionality
        ├── 📄 ksWifiConfigurator       ─── Handles WiFi configuration setup
        └── 📄 ksWifiConnector          ─── Manages WiFi connection
```

### 🔅 Rules

- Components should be added in the app's `init` method, so they will be available for `postInit` methods. (you can anytime later, from the `loop` but please treat it like exceptional case)
- The `init` method is the best place to add dependent components, setup initial pin values etc.
- The `postInit` method is the best place to obtain a weak pointer to another component by calling `findComponent`. This will handle cases when other components were added via `init` method.

## 🌱 Building the Application

To create an application, define a new class that inherits from `ksApplication` and add initial components in the `init` method. Refer to projects like [**emon_fw**](https://github.com/cziter15/emon_fw) for a practical example.

### 🔎 How It Works

- The application is instantiated, and its `init` method is called. If `init` returns `false`, the `loop` method is skipped, and the App Rotator proceeds to instantiate and run the next application in its sequence.
- If `init` returns `true`, the `loop` method executes, initializing all components.
- In the next iteration, each component’s `postInit` method is invoked.
- Once initialized, the application enters a continuous loop, calling each component’s `loop` method.
- If any component’s `loop` method returns `false`, the application terminates, and the App Rotator proceeds to the next application.

```cpp
bool PelletInfo::init()
{
    // Add WiFi and MQTT debug components
    addComponent<ksf::comps::ksWifiConnector>(PelletInfoConfig::pelletInfoDeviceName);
    addComponent<ksf::comps::ksMqttDebugResponder>();
    addComponent<ksf::comps::ksDevStatMqttReporter>();

    // Add OTA updater component
    addComponent<ksf::comps::ksDevicePortal>();

    // Add state display and receiver components
    addComponent<comps::StateDisplay>();
    addComponent<comps::StateReceiver>();

    // Add reset button component
    addComponent<ksf::comps::ksResetButton>(CFG_PUSH_PIN, LOW);

    // Add MQTT connector component
    addComponent<ksf::comps::ksMqttConnector>();

    // Initialization completed; return true to indicate success
    return true;
}
```

## 🔁 Application rotator

The library provides a very useful utility called `ksAppRotator`. This object can wrap application instantiation logic into a carousel-like rotation mechanism.

Typically, a device hosts two applications:

- Core application - Runs the core device logic.
- Configuration assistant - Assists the user with device configuration.

Each application implements its own `loop()` method to manage its logic. In case of a failure at any point (including during the application’s construction), the rotator will seamlessly switch to the next application and begin executing its logic.

This design is highly flexible. For example, you can trigger a failure (`return false`) during an application’s `init()` method, allowing the system to immediately switch into configuration mode if conditions require it (e.g. missing WiFi credentials).

## 🔣 Compiler flags

- Bare Arduino projects need to have `gnu++2a` enabled via `compiler.cpp.extra_flags=` option in the `board.txt` file.

## #️⃣ Custom RTTI

- Use the `KSF_RTTI_DECLARATIONS` macro to provide proper runtime type information generation for proper casting of components. 
- See `ksConfigProvider.h` for an example. Your application components should use this macro, otherwise the component finding mechanism won't work.

## 🔥 Saving power

- Modem sleep is enabled by default and can be controlled as an optional parameter in the `ksWifiConnector` constructor.
- Automatic modem sleep requires the DTIM _(Delivery Traffic Indication Message)_ to be correctly set on the access point. 
- The best value for me was `3`. It allows the ESP32 to go down from around 100mA to 20mA.

## 📑 Dependencies

- **It is highly recommended to use PlatformIO as it will automatically download dependencies!**

### 🔡 Frameworks

- [Arduino for ESP32](https://github.com/espressif/arduino-esp32)
- [Arduino for ESP8266](https://github.com/esp8266/Arduino)

### 🔡 Libraries

- [PubSubClient](https://github.com/knolleary/pubsubclient)
- [arduinoWebSockets](https://github.com/Links2004/arduinoWebSockets)

# GitHub Copilot Instructions for ksIotFrameworkLib

## Project Overview

ksIotFrameworkLib is an Arduino-based C++ IoT framework for ESP32 and ESP8266 microcontrollers. It provides:
- Composition-oriented architecture with component system
- WiFi setup and connectivity management
- MQTT telemetry protocol support
- Web-based device portal with OTA updates
- Configuration management system

## Technology Stack

- **Language**: C++ (C++20/gnu++2a standard)
- **Platforms**: ESP32 (Arduino 3+), ESP8266 (SDK305)
- **Build System**: PlatformIO
- **Key Dependencies**: pubsubclient3, arduinoWebSockets, LittleFS
- **Framework**: Arduino

## Commit Message Format

When creating commits for this repository, follow CONTRIBUTING.MD.
Your commit messages will appear in release notes, so write them with end users in mind.

### Commit Types

Use one of these types with parentheses format: `(type) Description`

- **feat**: Adds a new feature or capability
  - Example: `(feat) Add WiFi reconnection logic`
- **fix**: Resolves a bug or issue
  - Example: `(fix) Handle MQTT reconnect crash #12`
- **docs**: Updates or improves documentation
  - Example: `(docs) Update README with setup steps`
- **chore**: Maintenance or build-related task
  - Example: `(chore) Update dependency versions`
- **perf**: Improves performance or efficiency
  - Example: `(perf) Reduce memory usage in parser`
- **refactor**: Code cleanup or structure change
  - Example: `(refactor) Simplify config loader`
- **test**: Adds or updates tests
  - Example: `(test) Add unit tests for network manager`

### Commit Tips

- Be concise and specific
- Reference issues when possible (e.g., `#12`)
- Write for end users who will see these in release notes

## Coding Style & Conventions

### Naming Conventions

- **Classes**: PascalCase with `ks` prefix (e.g., `ksComponent`, `ksApplication`, `ksConfig`)
- **Namespaces**: Lowercase (e.g., `ksf`, `ksf::comps`, `ksf::misc`)
- **Functions/Methods**: camelCase (e.g., `initializeFramework`, `saveCredentials`)
- **Variables**: camelCase (e.g., `configPath`, `isDirty`)
- **Constants**: UPPERCASE with underscores (e.g., `NVS_DIRECTORY`, `OTA_FILENAME_TEXT`)
- **Enums**: Namespaced enum classes (e.g., `EOTAType::Type`, `EConfigParamType::Type`)

### Code Organization

- Use `namespace ksf` for framework code
- Use `namespace ksf::comps` for component classes
- Use `namespace ksf::misc` for utility classes
- Header files use `.h` extension
- Implementation files use `.cpp` extension
- All headers should have include guards using `#pragma once`

### Memory Management

- Prefer stack allocation over heap when possible
- Use `std::array` for fixed-size buffers
- Use `std::string` and `std::string_view` instead of C-style strings
- Use RAII principles for resource management
- Be mindful of memory constraints on embedded devices

### Modern C++ Features

- Use C++20 features where appropriate
- Prefer `auto` for type inference when it improves readability
- Use range-based for loops
- Use structured bindings
- Use `std::move` for move semantics
- Use `constexpr` for compile-time constants

### Arduino/ESP Specific

- Use `PROGMEM` macro for flash storage of constants
- Use `PSTR()` macro for flash strings
- Access LittleFS for file operations
- Be aware of ESP32 vs ESP8266 differences
- Follow Arduino framework conventions

### Configuration Files

- Configuration files are stored in `/nvs` directory (LittleFS)
- Use `ksConfig` class for configuration file handling
- Use `USING_CONFIG_FILE` macro for scoped file operations
- Configuration format: key-value pairs, alternating lines

### Component Pattern

- Components inherit from `ksComponent` base class
- Components declare RTTI using `KSF_RTTI_DECLARATIONS` macro
- Components can be composed into applications
- Follow the lifecycle methods: `init()`, `loop()`, etc.

## Documentation Style

- Use Doxygen-style comments for documentation
- Document all public APIs
- Use `@brief` for brief descriptions
- Use `@param` for parameters
- Use `@return` for return values
- Include usage examples where appropriate

### Example Comment Style

```cpp
/*!
    @brief Loads WiFi credentials from flash.
    @param ssid SSID reference to be loaded.
    @param password Password reference to be loaded.
*/
extern void loadCredentials(std::string& ssid, std::string& password);
```

## Build & Testing

- Use PlatformIO for building and testing
- Build configuration is in `platformio.ini` files
- Library-specific build settings in `scripts/setup_library_env.py`
- Examples are located in `examples/` directory
- Ensure builds pass before submitting PRs

## Project-Specific Patterns

### Configuration Provider Pattern

- Inherit from `ksConfigProvider` for configurable components
- Implement `readParams()` and `saveParams()` methods
- Use `addNewParam()` or `addNewParamWithConfigDefault()` to define parameters

### Framework Initialization

- Always call `KSF_FRAMEWORK_INIT()` at the start of `setup()`
- This initializes the framework and sets up necessary resources

### OTA Handling

- Framework automatically handles OTA boot indicator
- Check boot type with appropriate functions
- Clean up indicator files after OTA

## File Locations

- Source code: `src/ksf/`
- Components: `src/ksf/comp/`
- Utilities: `src/ksf/misc/`
- Examples: `examples/`
- Build scripts: `scripts/`

## Additional Resources

- Documentation: https://cziter15.github.io/ksIotFrameworkLib
- Examples: https://github.com/cziter15/ksIotFrameworkLib/tree/master/examples
- Wiki: https://github.com/cziter15/ksIotFrameworkLib/wiki

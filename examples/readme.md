# Examples

In this directory, you will find examples that guide you through building various firmware applications.

**Before you start, read this page to ensure you are familiar with PlatformIO and have the correct IDE setup.**

## IDE Preparation

**When using PlatformIO, no additional setup is required, and you don't need to install the Arduino IDE separately. This library can be used with the Arduino IDE, though it may be suboptimal.**

- [Download and install official Microsoft Visual Studio Code](https://code.visualstudio.com/). PlatformIO IDE is built on top of it.
- Open the extension manager.
- Search for the official `platformio ide` extension.
- Install the PlatformIO IDE extension.

![PlatformIO Extension](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/18dfb3a2-866c-4308-a197-5a90bbb13b90)

## Opening Existing Projects

- If properly configured, the project should contain a `platformio.ini` file in its main directory.
- To open a project, use the `Open Folder` option from the File menu.
- Dependencies will be automatically installed, which may take some time.

## Creating Your First Application

1. Open PlatformIO UI.

2. Click on `New project`, and select your ESP version. Then click `Finish`.

   ![New Project](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/494565fa-4753-446c-a18e-c903626637a3)

3. Open the `platformio.ini` file:

   ![platformio.ini](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/1d882165-c64f-40a6-addb-36141837c5dc)

4. Add the following code and save the file:

   ```ini
   # Define library dependencies.
   lib_deps = 
       ksIotFrameworkLib=https://github.com/cziter15/ksIotFrameworkLib
   ```

5. PlatformIO will download the required dependencies. Please be patient, as it may take some time.

6. Now open the `main.cpp` file, replace the code with the following, and save the file:

   ```cpp
   #include <ksIotFrameworkLib.h>

   class MyApplication : public ksf::ksApplication
   {
   public:
       bool init() override 
       {
           return true;
       }

       bool loop() override 
       {
           return true;
       }
   };

   KSF_IMPLEMENT_APP_ROTATOR
   (
       MyApplication
   )

7. Now you are able to build the application stub.

   ![image](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/afac0511-cf68-4007-ba89-b2902cabca6c)

8. Now all that's left to do is implement the components and logic of your device. 

## Available Examples

### [led-blink](led-blink) 🔰 **Start Here!**

A comprehensive tutorial that teaches the fundamentals of ksIotFrameworkLib through a simple LED blinking application. **Perfect for beginners!**

**What you'll learn:**

- Creating applications and using components
- Managing configuration with persistent storage
- Implementing automatic application rotation
- Building a web-based configuration interface
- Controlling hardware (GPIO pins and LEDs)

**Features:**

- User-configurable LED blink interval
- WiFi connectivity with automatic reconnection
- Web-based configuration portal
- Extensively documented code

This example is designed as a tutorial with detailed explanations of every concept, making it the ideal starting point for learning the framework.

### [basic-config](basic-config)

This application demonstrates how to create a typical IoT application stack with MQTT connectivity. One ksApplication handles device logic while another provides configuration capabilities.

**Features:**

- MQTT integration for device communication
- Device status reporting
- WiFi configuration interface
- Multiple application pattern

# Tutorials
Before using ksIotFrameworkLib, ensure you are familiar with PlatformIO and have the correct IDE setup.

## IDE Preparation

**When using PlatformIO, no additional setup is required, and you don't need to install the Arduino IDE separately. This library can be used with the Arduino IDE, though it may be suboptimal.**

- [Download and install official Microsoft Visual Studio Code](https://code.visualstudio.com/). PlatformIO IDE is built on top of it.
- Open the extension manager.
- Search for the official `platformio ide` extension.
- Install PlatformIO IDE extension.

![image](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/5f67669d-1f18-4be6-a251-c7b1498387c5)

## Opening existing projects
- If properly configured, the project should contain a `platformio.ini` file in its main directory.
- To open a project, use the `Open Folder` option from the File menu.
- Dependencies will be automatically installed, which may take some time.

## Creating your first application
1. Open PlatformIO UI.
2. Click on `New project`, and select your ESP version. Then click `Finish`.

![image](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/494565fa-4753-446c-a18e-c903626637a3)

4. Open the `platformio.ini` file:

![image](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/15565b44-8f44-4f7d-b63a-041573234e1f)

6. Add the following code and save the file:
```ini
# Define library dependencies.
lib_deps = 
	ksIotFrameworkLib=https://github.com/cziter15/ksIotFrameworkLib
```
6. Platformio will download required dependencies. Please be patient as it may take some time.
8. Now open `main.cpp` file and replace the code with the following listing and save the file.
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
```

8. Now you are able to build the application stub.

![image](https://github.com/cziter15/ksIotFrameworkLib/assets/5003708/afac0511-cf68-4007-ba89-b2902cabca6c)

10. Now all that's left to do is implement the components and logic of your device. 

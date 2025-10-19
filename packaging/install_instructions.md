# OpenRGB Hue Entertainment Plugin Installation

## 1. Build the Plugin

Follow the build instructions in the main `README.md` file to compile the plugin for your operating system.

## 2. Locate Your OpenRGB Plugins Directory

The location of the OpenRGB plugins directory varies by operating system:

*   **Linux:** `~/.config/OpenRGB/plugins`
*   **Windows:** `%APPDATA%\OpenRGB\plugins`

## 3. Copy the Plugin File

Copy the compiled plugin file to the plugins directory:

*   **Linux:** Copy `libOpenRGB_HueEntertainment.so` to `~/.config/OpenRGB/plugins`.
*   **Windows:** Copy `OpenRGB_HueEntertainment.dll` to `%APPDATA%\OpenRGB\plugins`.

## 4. Restart OpenRGB

Start or restart OpenRGB. The plugin will be loaded automatically.

## 5. Configuration

The plugin will create a default configuration file when it runs for the first time. You will need to follow the push-link authentication process as described in the `README.md` to connect the plugin to your Hue Bridge.

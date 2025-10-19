# OpenRGB Hue Entertainment Plugin

This plugin for OpenRGB allows you to synchronize your OpenRGB-controlled devices with a Philips Hue Entertainment Area. It uses the Hue Entertainment API v2, communicating directly with the Hue Bridge via a DTLS-secured connection.

## Features

-   **Direct Hue Entertainment API v2 Integration:** Low-latency, high-performance streaming directly to your Hue Bridge.
-   **Cross-Platform:** Works on both Linux and Windows.
-   **Flexible LED Mapping:** Map individual OpenRGB LEDs or zones to your Hue lights.
-   **Secure:** Uses a DTLS-PSK secured connection to the Hue Bridge.

## Build Instructions

### Linux

1.  **Install Dependencies:**
    ```bash
    sudo apt-get install build-essential cmake
    ```
2.  **Build the Plugin:**
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```

### Windows

1.  **Install Dependencies:**
    -   Install [Visual Studio](https://visualstudio.microsoft.com/) with the "Desktop development with C++" workload.
    -   Install [CMake](https://cmake.org/download/).
2.  **Build the Plugin:**
    -   Open a developer command prompt.
    -   Navigate to the project directory.
    -   Run cmake:
        ```bash
        mkdir build
        cd build
        cmake ..
        cmake --build . --config Release
        ```

## Installation

1.  Copy the built plugin (`libOpenRGB_HueEntertainment.so` on Linux, `OpenRGB_HueEntertainment.dll` on Windows) to the `plugins` directory of your OpenRGB installation.
2.  Start OpenRGB.

## Configuration and Push-Link Authentication

1.  The first time you run the plugin, it will attempt to register with your Hue Bridge.
2.  You will be prompted in the OpenRGB log to press the button on your Hue Bridge.
3.  Once you press the button, the plugin will save your credentials to a configuration file:
    -   **Linux:** `~/.config/openrgb-hue-entertainment/config.json`
    -   **Windows:** `%APDATA%/openrgb-hue-entertainment/config.json`
4.  You can then edit this file to configure your entertainment area and LED mappings.

## Troubleshooting

-   **Firewall:** Ensure that UDP port 2100 is not blocked on your firewall.
-   **MTU:** If you experience packet loss, you may need to adjust the MTU size on your network interface.
-   **Logs:** Check the OpenRGB logs for detailed information about the plugin's status and any errors.

## Example Log Output

```
[info] Initializing OpenRGB Hue Entertainment Plugin
[info] Press the button on the Hue Bridge...
[info] Successfully registered with Hue Bridge. Username: your-username
[info] DTLS handshake successful.
[info] Starting stream.
```

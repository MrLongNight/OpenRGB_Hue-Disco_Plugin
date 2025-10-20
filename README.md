# OpenRGB Hue Entertainment Plugin

This plugin for OpenRGB allows you to synchronize your OpenRGB-controlled devices with a Philips Hue Entertainment Area. It uses the Hue Entertainment API v2, communicating directly with the Hue Bridge via a DTLS-secured connection.

## Features

-   **Direct Hue Entertainment API v2 Integration:** Low-latency, high-performance streaming directly to your Hue Bridge.
-   **Cross-Platform:** Works on both Linux and Windows.
-   **Seamless Integration:** Uses the main OpenRGB configuration file for Hue Bridge details.

## Dependencies

The plugin uses `CMake` with `FetchContent` to download and build its dependencies automatically. For this to work, you need the necessary development libraries installed on your system.

-   **mbedtls**
-   **nlohmann_json**
-   **spdlog**
-   **cpp-httplib** (with OpenSSL support)
-   **Catch2** (*For tests only*)

## Build Instructions

### Linux

1.  **Install Dependencies:**
    ```bash
    # Debian / Ubuntu
    sudo apt-get install build-essential cmake libssl-dev
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
    -   Install a C++ compiler (e.g., Visual Studio, MinGW).
    -   Install CMake.
    -   Install OpenSSL.
2.  **Build the Plugin:**
    ```bash
    mkdir build
    cd build
    cmake ..
    cmake --build . --config Release
    ```

## Installation

1.  Copy the built plugin (`libOpenRGB_HueEntertainment.so` on Linux, `OpenRGB_HueEntertainment.dll` on Windows) to the `plugins` directory of your OpenRGB installation.
2.  Start OpenRGB.

## Configuration

This plugin does not have its own configuration file. Instead, it reads the main OpenRGB configuration file to find your Hue Bridge.

1.  **Configure the Hue Bridge in OpenRGB:**
    -   Go to the "Settings" tab in OpenRGB.
    -   Go to the "Devices" tab.
    -   Select the "Philips Hue" detector and configure your bridge.
2.  **Enable Entertainment Mode:**
    -   In the Hue Bridge settings within OpenRGB, make sure to select an Entertainment Area and check the "Enable Entertainment Mode" box.
3.  **Use the Plugin:**
    -   Go to the "Plugins" tab.
    -   Select the "Hue Entertainment" plugin.
    -   If you have multiple Hue Bridges configured for entertainment, select the one you want to use from the dropdown menu.
    -   Click "Start Streaming".

## Troubleshooting

-   **Firewall:** Ensure that outgoing UDP traffic to your Hue Bridge (port 2100) is not blocked.
-   **Logs:** Check the OpenRGB logs for detailed information about the plugin's status and any errors.

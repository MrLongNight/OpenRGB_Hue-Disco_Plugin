# OpenRGB Hue Entertainment Plugin

This plugin for OpenRGB allows you to synchronize your OpenRGB-controlled devices with a Philips Hue Entertainment Area. It uses the Hue Entertainment API v2, communicating directly with the Hue Bridge via a DTLS-secured connection.

## Features

-   **Direct Hue Entertainment API v2 Integration:** Low-latency, high-performance streaming directly to your Hue Bridge.
-   **Cross-Platform:** Works on both Linux and Windows.
-   **Automatic Bridge Discovery:** Finds your Hue Bridge on the network automatically.
-   **Flexible LED Mapping:** Map individual OpenRGB LEDs or zones to your Hue lights.
-   **Secure:** Uses a DTLS-PSK secured connection to the Hue Bridge and stores credentials securely.

## Dependencies

The plugin uses `CMake` with `FetchContent` to download and build its dependencies automatically. For this to work, you need the necessary development libraries installed on your system.

-   **mbedtls** (v3.1.0)
-   **nlohmann_json** (v3.11.2)
-   **spdlog** (v1.11.0)
-   **cpp-httplib** (v0.11.3)
-   **Catch2** (v3.3.2) - *For tests only*

Additionally, `cpp-httplib` requires OpenSSL for the bridge discovery feature.

## Build Instructions

### CI Builds (Recommended)

The easiest way to get the plugin is to download the latest build artifact from the [Actions](https://github.com/your-repo/openrgb-hue-entertainment-plugin/actions) page of this repository. These builds are created automatically and are ready to use.

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

### Windows (with vcpkg)

Using `vcpkg` is the recommended way to manage dependencies on Windows.

1.  **Install Dependencies:**
    -   Install [Visual Studio 2022](https://visualstudio.microsoft.com/) with the "Desktop development with C++" workload.
    -   Install [CMake](https://cmake.org/download/).
    -   Install [vcpkg](https://vcpkg.io/en/getting-started.html) and integrate it with Visual Studio (`vcpkg integrate install`).
    -   Install the required libraries via vcpkg:
        ```bash
        vcpkg install mbedtls nlohmann-json spdlog cpp-httplib[openssl] catch2
        ```
2.  **Build the Plugin:**
    -   Open a developer command prompt.
    -   Navigate to the project directory.
    -   Run CMake, pointing it to the vcpkg toolchain file:
        ```bash
        mkdir build
        cd build
        cmake .. -DCMAKE_TOOLCHAIN_FILE=[path-to-vcpkg]/scripts/buildsystems/vcpkg.cmake
        cmake --build . --config Release
        ```

## Installation

1.  Copy the built plugin (`libOpenRGB_HueEntertainment.so` on Linux, `OpenRGB_HueEntertainment.dll` on Windows) to the `plugins` directory of your OpenRGB installation.
2.  Start OpenRGB.

## Configuration and Security

1.  On the first run, the plugin will attempt to discover your Hue Bridge. If it fails, you can manually enter the IP address in the configuration file.
2.  The plugin will then prompt you in the OpenRGB log to press the physical button on your Hue Bridge to authorize the plugin.
3.  Once authorized, the plugin will save your credentials (username and clientkey) to a configuration file. This file is stored with secure permissions (`0600`) to protect it from other users on your system.
    -   **Linux:** `~/.config/openrgb-hue-entertainment/config.json`
    -   **Windows:** `%APPDATA%/openrgb-hue-entertainment/config.json`

### Revoking Credentials

If you wish to revoke the plugin's access to your Hue Bridge, you can do so from the Philips Hue mobile app:
1.  Go to **Settings > Entertainment Areas**.
2.  Select the entertainment area you are using.
3.  Tap on the "i" (info) icon.
4.  You will see a list of connected devices/apps. You can remove the "openrgb_hue_entertainment#plugin" from this list.

## Troubleshooting

-   **Firewall:** Ensure that outgoing connections to `discovery.meethue.com` (port 443) and outgoing UDP traffic to your Hue Bridge (port 2100) are not blocked.
-   **Logs:** Check the OpenRGB logs for detailed information about the plugin's status and any errors.

# OpenRGB Hue Entertainment Plugin Analysis & To-Do

This document provides a comprehensive analysis of the OpenRGB Hue Entertainment Plugin and a prioritized to-do list for future development.

## 1. Code Analysis (Unchanged)

The plugin is written in C++ and integrates with OpenRGB to synchronize lighting effects with Philips Hue Entertainment Areas. It uses the Hue Entertainment API v2 and communicates with the Hue Bridge over a DTLS-secured connection.

### Key Components:

-   **PluginHueEntertainment:** The main plugin class.
-   **HueClient:** Handles REST API communication with the Hue Bridge.
-   **DTLSClient:** Manages the DTLS connection and streaming.
-   **ConfigManager:** Handles configuration loading and saving.
-   **MappingEngine:** Maps OpenRGB LEDs to Hue lights.
-   **HueStreamer:** Manages the color data stream.
-   **HueDevice:** Represents the Hue Entertainment Area as an OpenRGB device.

### Overall Architecture:

The plugin follows a modular design with a clear separation of concerns. The use of modern C++ features and established libraries is a good practice.

## 2. Prioritized To-Do List

This section combines the initial analysis with a detailed technical audit. It is prioritized to address the most critical issues first.

### P0: Critical (Must Fix Now)

1.  **Make `SetLEDs` Non-Blocking (Performance)**
    *   **Problem:** `HueDevice::SetLEDs` calls `streamer_->updateColors()` synchronously. This can block the main OpenRGB thread, causing the UI to freeze, especially with high update rates or during DTLS reconnects.
    *   **Solution:** Implement a non-blocking producer/consumer model. `SetLEDs` (the producer) should write the latest color data to a lock-free slot (e.g., `std::atomic<std::shared_ptr<...>>`) and return immediately. The `HueStreamer` thread (the consumer) will then load the latest frame from this slot for processing, dropping any intermediate frames to prevent queue buildup.
    *   **Files:** `src/HueDevice.cpp`, `src/HueStreamer.h`, `src/HueStreamer.cpp`

2.  **Fix DTLS Payload Construction (Bug)**
    *   **Problem:** The `DTLSClient::sendFrame` function incorrectly constructs the data payload, writing the green and blue color channels twice. This results in corrupt frames that the Hue Bridge cannot interpret correctly.
    *   **Solution:** Correct the payload building sequence to ensure each color channel (R, G, B) is written once and in the correct order. The final format should match the Hue Entertainment API v2 specification (e.g., a binary format or a JSON payload). The MTU size should also be considered to avoid packet fragmentation.
    *   **File:** `src/DTLSClient.cpp`

3.  **Complete `HueStreamer` Consumer Loop (Incomplete Implementation)**
    *   **Problem:** The streaming logic in `HueStreamer` is a placeholder and incomplete. The core consumer loop, connection management, and frame pacing are missing.
    *   **Solution:** Fully implement the consumer thread loop to:
        *   Wait for new frames using a condition variable with a timeout for frame pacing (to match `target_fps`).
        *   Atomically load the latest frame.
        *   Call the `MappingEngine` to transform colors.
        *   Send the frame via the `DTLSClient`.
        *   Implement a robust reconnect mechanism with exponential backoff (e.g., 500ms, capping at 30s) if the DTLS connection fails.
    *   **File:** `src/HueStreamer.cpp`

### P1: High Priority (Next Steps)

1.  **Improve `MappingEngine` Logic (Bug / Quality)**
    *   **Problem:** The mapping logic uses integer math for averaging, leading to precision loss. It also lacks checks for division by zero if a mapping has no source LEDs.
    *   **Solution:** Refactor the `MappingEngine` to perform all color calculations using floating-point numbers (0.0 to 1.0). Add guards to prevent division by zero. Implement proper interpolation strategies for cases where the number of LEDs is smaller than the number of Hue lamps.
    *   **File:** `src/MappingEngine.cpp`

2.  **Robust `HueClient` REST Implementation (Incomplete Implementation)**
    *   **Problem:** The `HueClient` is largely a skeleton. Bridge discovery is a placeholder, and push-link authentication and entertainment area management are not robustly implemented.
    *   **Solution:** Implement the REST client functions using a reliable HTTP library (like cpp-httplib) with proper timeouts and error handling. The push-link process must correctly poll for the button press and parse the credentials from the response.
    *   **Files:** `src/HueClient.cpp`, `src/HueClient.h`

3.  **Fix CMake Dependency Management (Build)**
    *   **Problem:** The root `CMakeLists.txt` contains placeholders (`...`) instead of proper `FetchContent` blocks for dependencies like `nlohmann_json`, `spdlog`, and `httplib`. This will cause the build to fail.
    *   **Solution:** Replace the placeholders with complete `FetchContent_Declare` and `FetchContent_MakeAvailable` calls for all external dependencies. Ensure the target names used in `target_link_libraries` match those provided by `FetchContent`.
    *   **File:** `CMakeLists.txt`

### P2: Medium Priority (Future Improvements)

1.  **Improve Credential Security**
    *   **Problem:** Credentials are stored in a plain-text JSON file.
    *   **Solution:** Secure the configuration file by setting appropriate file permissions (e.g., `0600` on Linux/macOS) or using platform-specific APIs (like DPAPI on Windows). Mask credentials in logs.
    *   **File:** `src/ConfigManager.cpp`

2.  **Add Stress Test**
    *   **Task:** Create a new test case that simulates `SetLEDs` being called at a very high frequency (e.g., 200Hz) to verify that the implementation is truly non-blocking and does not cause performance issues.

3.  **Create CI/CD Pipeline**
    *   **Task:** Implement a GitHub Actions workflow that builds the plugin, preferably using `vcpkg` for dependency management. The workflow should produce the final `.dll` (Windows) or `.so` (Linux) as a build artifact.

4.  **Enhance Documentation (`README.md`)**
    *   **Task:** Update the `README.md` to include:
        *   An exact list of dependencies and versions for manual builds.
        *   Instructions on how to use the CI build artifacts.
        *   A sample configuration file with explanations.
        *   A note on credential security and how to revoke access on the Hue Bridge.

5.  **Implement a User Interface**
    *   **Task:** Design and implement a GUI integrated into the OpenRGB frontend for easier configuration of the bridge, entertainment areas, and LED mappings. (See GUI Proposal section).

## 3. GUI Proposal (Unchanged)

A GUI would significantly improve the plugin's usability. It should be integrated into the OpenRGB user interface and provide the following features:

### Tabs:

-   **Bridge:**
    -   A button to automatically discover the Hue Bridge on the network.
    -   A field to manually enter the bridge's IP address.
    -   A button to initiate push-link authentication.
-   **Entertainment Areas:**
    -   A list of available entertainment areas.
    -   A button to select the desired entertainment area.
-   **LED Mapping:**
    -   A visual representation of the OpenRGB LEDs and Hue lights.
    -   Drag-and-drop functionality to map LEDs to lights.
    -   The ability to save and load mapping profiles.
-   **Settings:**
    -   Options to configure the plugin's logging level.
    -   The ability to adjust the streaming rate.

## 4. Plugin Potential (Unchanged)

With the suggested improvements, the OpenRGB Hue Entertainment Plugin could become a powerful tool for creating immersive lighting experiences.

### Hue Entertainment Control:

-   **Real-time Synchronization:** The plugin could synchronize OpenRGB effects with Hue lights in real-time, creating a seamless lighting experience across all devices.
-   **Dynamic Effects:** The plugin could be extended to support dynamic effects, such as a "screen bloom" effect that mirrors the colors on the user's screen.
-   **Audio Visualization:** The plugin could be integrated with an audio visualizer to create lighting effects that react to music.

### OpenRGB Effect Control:

-   **Hue as a Controller:** The plugin could allow users to control OpenRGB devices with the Hue app.
-   **Scene Support:** The plugin could synchronize OpenRGB effects with Hue scenes.
-   **Routine Integration:** The plugin could be integrated with Hue routines to automate lighting effects.

## 5. GUI Mockup (Unchanged)

Here is a simple text-based mockup of what the GUI could look like:

```
+-----------------------------------------------------------------+
| OpenRGB Hue Entertainment Plugin                                |
+-----------------------------------------------------------------+
| [ Bridge ] [ Entertainment Areas ] [ LED Mapping ] [ Settings ]   |
+-----------------------------------------------------------------+
|                                                                 |
|  Bridge IP: [ 192.168.1.100       ] [ Discover ]                 |
|                                                                 |
|  [ Authenticate with Bridge ]                                   |
|                                                                 |
|  Status: [ Connected to Hue Bridge ]                            |
|                                                                 |
+-----------------------------------------------------------------+
```

## 6. Open Questions (New)

-   **Hue API Responses:** The exact format of the Hue v2 CLIP API response for `generateclientkey` (e.g., base64, hex) needs to be confirmed to implement the parser correctly.
-   **Entertainment Frame Format:** The optimal payload format for the DTLS stream (binary vs. JSON) should be determined by consulting the Hue documentation to ensure the lowest latency.
-   **Error Handling & Mapping:** A robust mapping between OpenRGB's internal device/LED ordering and the Hue lamp UUIDs needs to be designed, including error handling for mismatched configurations.

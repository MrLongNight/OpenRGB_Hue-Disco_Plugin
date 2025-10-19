add_library(OpenRGB_HueEntertainment SHARED
    PluginHueEntertainment.cpp
    HueDevice.cpp
    HueClient.cpp
    DTLSClient.cpp
    MappingEngine.cpp
    HueStreamer.cpp
    ConfigManager.cpp
    Logger.cpp
    plugin_mock.cpp
)

target_include_directories(OpenRGB_HueEntertainment PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(OpenRGB_HueEntertainment PUBLIC
    mbedtls
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib
)

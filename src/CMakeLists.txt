# Find Qt6 - it might not be available, so find it quietly
find_package(Qt6 COMPONENTS Widgets QUIET)

# Enable AUTOMOC for Qt integration
set(CMAKE_AUTOMOC ON)

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

# Add GUI files only if Qt was found
if(Qt6_FOUND)
    target_sources(OpenRGB_HueEntertainment PRIVATE
        HuePluginWidget.cpp
    )
endif()

target_include_directories(OpenRGB_HueEntertainment PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(OpenRGB_HueEntertainment PUBLIC
    mbedcrypto
    mbedx509
    mbedtls
    nlohmann_json::nlohmann_json
    spdlog::spdlog
    httplib
    ssl
    crypto
)

# Link Qt only if it was found
if(Qt6_FOUND)
    target_link_libraries(OpenRGB_HueEntertainment PUBLIC Qt6::Widgets)
    target_compile_definitions(OpenRGB_HueEntertainment PRIVATE WITH_QT_WIDGETS)
endif()

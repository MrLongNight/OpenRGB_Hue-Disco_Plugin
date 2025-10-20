cmake_minimum_required(VERSION 3.16)
project(OpenRGBHueEntertainmentPlugin LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

enable_testing()

include(FetchContent)

FetchContent_Declare(
  mbedtls
  GIT_REPOSITORY https://github.com/Mbed-TLS/mbedtls.git
  GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(mbedtls)

FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.11.0
)
FetchContent_MakeAvailable(spdlog)

FetchContent_Declare(
  httplib
  GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
  GIT_TAG v0.11.3
)
FetchContent_MakeAvailable(httplib)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.3.2
)
FetchContent_MakeAvailable(Catch2)

add_subdirectory(src)
add_subdirectory(tests)

set(OPENRGB_PLUGIN_INSTALL_DIR "${CMAKE_INSTALL_PREFIX}/plugins" CACHE PATH "Directory to install OpenRGB plugins")
install(TARGETS OpenRGB_HueEntertainment DESTINATION ${OPENRGB_PLUGIN_INSTALL_DIR})

#include <catch2/catch_all.hpp>
#include "HueDevice.h"
#include <vector>
#include <chrono>
#include <cstdint>

TEST_CASE("SetLEDs is non-blocking under stress", "[StressTest]") {
    // Setup a mock HueDevice
    int num_leds = 10;
    HueDevice device(num_leds);
    std::vector<Color> colors(num_leds, {255, 0, 0});

    int num_calls = 1000;

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_calls; ++i) {
        device.SetLEDs(colors);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // If SetLEDs were blocking, the duration would be in the order of seconds (due to streamer delays).
    // A non-blocking implementation should be in the order of a few milliseconds for 1000 calls.
    // We set a generous threshold of 100ms to account for test overhead.
    REQUIRE(duration < 100);
}

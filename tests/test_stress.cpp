#include <catch2/catch_all.hpp>
#include "HueDevice.h"
#include <vector>
#include <chrono>

TEST_CASE("SetLEDs is non-blocking under stress", "[StressTest]") {
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

    REQUIRE(duration < 100);
}

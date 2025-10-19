#include <catch2/catch_all.hpp>
#include "MappingEngine.h"

TEST_CASE("MappingEngine 1:1 Mapping", "[MappingEngine]") {
    MappingEngine engine;
    std::vector<Color> in = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};
    std::vector<HueColor> out;
    out.resize(3);

    engine.mapOpenRGBtoHue(in, out);

    REQUIRE(out.size() == 3);
    REQUIRE(out[0].r == 255);
    REQUIRE(out[0].g == 0);
    REQUIRE(out[0].b == 0);
    REQUIRE(out[1].r == 0);
    REQUIRE(out[1].g == 255);
    REQUIRE(out[1].b == 0);
}

TEST_CASE("MappingEngine Averaging Mapping", "[MappingEngine]") {
    MappingEngine engine;
    std::vector<Mapping> mappings = {
        { {0, 1}, "lamp1" },
        { {2, 3}, "lamp2" }
    };
    engine.loadMapping(mappings);

    std::vector<Color> in = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}};
    std::vector<HueColor> out;

    engine.mapOpenRGBtoHue(in, out);

    REQUIRE(out.size() == 2);
    REQUIRE(out[0].r == 127);
    REQUIRE(out[0].g == 127);
    REQUIRE(out[0].b == 0);
    REQUIRE(out[1].r == 127);
    REQUIRE(out[1].g == 127);
    REQUIRE(out[1].b == 127);
}

TEST_CASE("MappingEngine Interpolation Mapping", "[MappingEngine]") {
    MappingEngine engine;
    std::vector<Color> in = {{255, 0, 0}, {0, 0, 255}};
    std::vector<HueColor> out;
    out.resize(3);

    engine.mapOpenRGBtoHue(in, out);

    REQUIRE(out.size() == 3);
    REQUIRE(out[0].r == 255);
    REQUIRE(out[0].g == 0);
    REQUIRE(out[0].b == 0);
    REQUIRE(out[1].r == 127);
    REQUIRE(out[1].g == 0);
    REQUIRE(out[1].b == 127);
    REQUIRE(out[2].r == 0);
    REQUIRE(out[2].g == 0);
    REQUIRE(out[2].b == 255);
}

TEST_CASE("MappingEngine Downsampling Mapping", "[MappingEngine]") {
    MappingEngine engine;
    std::vector<Color> in = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}};
    std::vector<HueColor> out;
    out.resize(2);

    engine.mapOpenRGBtoHue(in, out);

    REQUIRE(out.size() == 2);
    REQUIRE(out[0].r == 127);
    REQUIRE(out[0].g == 127);
    REQUIRE(out[0].b == 0);
    REQUIRE(out[1].r == 127);
    REQUIRE(out[1].g == 127);
    REQUIRE(out[1].b == 127);
}

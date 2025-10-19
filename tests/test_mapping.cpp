#include <catch2/catch_all.hpp>
#include "MappingEngine.h"
#include <vector>

using Catch::Matchers::WithinAbs;

TEST_CASE("MappingEngine 1:1 Mapping", "[MappingEngine]") {
    std::vector<std::string> lamp_uuids = {"lamp1", "lamp2", "lamp3"};
    MappingEngine engine(lamp_uuids);
    std::vector<Color> in = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}};
    std::vector<MappedHueColor> out;

    engine.mapOpenRGBtoHue(in, out);

    REQUIRE(out.size() == 3);
    CHECK_THAT(out[0].r, WithinAbs(1.0, 0.001));
    CHECK_THAT(out[0].g, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[0].b, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].r, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].g, WithinAbs(1.0, 0.001));
    CHECK_THAT(out[1].b, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[2].r, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[2].g, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[2].b, WithinAbs(1.0, 0.001));
}

TEST_CASE("MappingEngine Downsampling (Segment Average)", "[MappingEngine]") {
    std::vector<std::string> lamp_uuids = {"lamp1", "lamp2"};
    MappingEngine engine(lamp_uuids);

    std::vector<Color> in = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 0}};
    std::vector<MappedHueColor> out;

    engine.mapOpenRGBtoHue(in, out);

    // Lamp 1 should be average of LED 1 and 2 ({255,0,0}, {0,255,0}) -> {127.5, 127.5, 0} -> {0.5, 0.5, 0.0}
    // Lamp 2 should be average of LED 3 and 4 ({0,0,255}, {255,255,0}) -> {127.5, 127.5, 127.5} -> {0.5, 0.5, 0.5}
    REQUIRE(out.size() == 2);
    CHECK_THAT(out[0].r, WithinAbs(0.5, 0.001));
    CHECK_THAT(out[0].g, WithinAbs(0.5, 0.001));
    CHECK_THAT(out[0].b, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].r, WithinAbs(0.5, 0.001));
    CHECK_THAT(out[1].g, WithinAbs(0.5, 0.001));
    CHECK_THAT(out[1].b, WithinAbs(0.5, 0.001));
}

TEST_CASE("MappingEngine Upsampling (Interpolation)", "[MappingEngine]") {
    std::vector<std::string> lamp_uuids = {"lamp1", "lamp2", "lamp3"};
    MappingEngine engine(lamp_uuids);
    std::vector<Color> in = {{255, 0, 0}, {0, 0, 255}};
    std::vector<MappedHueColor> out;

    engine.mapOpenRGBtoHue(in, out);

    // According to the implementation:
    // Lamp 1 (pos 0.33) -> LED index 0
    // Lamp 2 (pos 1.0) -> LED index 1
    // Lamp 3 (pos 1.66) -> LED index 1
    REQUIRE(out.size() == 3);
    CHECK_THAT(out[0].r, WithinAbs(1.0, 0.001));
    CHECK_THAT(out[0].g, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[0].b, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].r, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].g, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[1].b, WithinAbs(1.0, 0.001));
    CHECK_THAT(out[2].r, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[2].g, WithinAbs(0.0, 0.001));
    CHECK_THAT(out[2].b, WithinAbs(1.0, 0.001));
}

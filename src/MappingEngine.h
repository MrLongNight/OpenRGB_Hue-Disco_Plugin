#pragma once

#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include "OpenRGB/Device.h"

struct MappedHueColor { float r, g, b; std::string lamp_uuid; };

class MappingEngine {
public:
    MappingEngine(const std::vector<std::string>& lamp_uuids);
    void mapOpenRGBtoHue(const std::vector<Color>& openrgb_leds, std::vector<MappedHueColor>& hue_frame);

private:
    std::vector<std::string> lamp_uuids_; // deterministische Reihenfolge

    void map_1to1(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame);
    void map_segment_average(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame);
    void map_interpolate(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame);

    static void color_to_float(const Color& in, float& r, float& g, float& b);
};

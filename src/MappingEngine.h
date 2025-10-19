#pragma once

#include "ConfigManager.h"
#include "DTLSClient.h" // For HueColor
#include "OpenRGB/Device.h" // For Color
#include <vector>

class MappingEngine {
public:
    void loadMapping(const std::vector<Mapping>& mappings);
    void mapOpenRGBtoHue(const std::vector<Color>& in, std::vector<HueColor>& out);

private:
    std::vector<Mapping> mappings_;
};

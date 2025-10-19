#pragma once

#include <vector>
#include <cstdint>

struct Color {
    uint8_t r, g, b;
};

class Device {
public:
    virtual ~Device() = default;
    virtual void SetLEDs(const std::vector<Color>& colors) = 0;
};

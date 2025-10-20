#pragma once

#include <vector>
#include <string>
#include <cstdint>

struct Color {
    uint8_t r, g, b;
};

struct Zone {
    std::vector<Color> leds_;
};

class Device {
public:
    virtual ~Device() = default;
    virtual void SetLEDs(const std::vector<Color>& colors) = 0;

    std::string device_name_;
    std::string device_type_;
    std::string vendor_;
    std::vector<Zone> zones_;
};

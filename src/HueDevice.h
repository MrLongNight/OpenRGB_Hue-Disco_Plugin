#pragma once

#include "OpenRGB/Device.h"
#include <memory>
#include <vector>

class HueStreamer; // Forward declaration

class HueDevice : public Device {
public:
    HueDevice(HueStreamer* streamer, int num_leds);
    
    void SetLEDs(const std::vector<Color>& colors) override;

private:
    HueStreamer* streamer_;
};

#pragma once

#include "OpenRGB/Device.h"
#include "LatestFrame.h"
#include <memory>
#include <vector>

class HueDevice : public Device {
public:
    HueDevice(int num_leds);
    
    void SetLEDs(const std::vector<Color>& colors) override;
    
    LatestSlot<std::vector<Color>> latest_slot_;
};

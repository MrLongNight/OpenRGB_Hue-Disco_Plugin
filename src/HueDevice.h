#pragma once

#include "OpenRGB/Device.h"
#include "HueStreamer.h"
#include <memory>

class HueDevice : public Device {
public:
    HueDevice(std::shared_ptr<HueStreamer> streamer, int num_leds);
    
    void SetLEDs(const std::vector<Color>& colors) override;
    
private:
    std::shared_ptr<HueStreamer> streamer_;
};

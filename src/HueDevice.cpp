#include "HueDevice.h"

HueDevice::HueDevice(std::shared_ptr<HueStreamer> streamer, int num_leds)
    : streamer_(streamer) {
    // Initialize device properties based on OpenRGB API
    // this->name = "Hue Entertainment Area";
    // this->leds.resize(num_leds);
}

void HueDevice::SetLEDs(const std::vector<Color>& colors) {
    streamer_->updateColors(colors);
}

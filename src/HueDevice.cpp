#include "HueDevice.h"

HueDevice::HueDevice(int num_leds) {
    // Initialize device properties based on OpenRGB API
    // this->name = "Hue Entertainment Area";
    // this->leds.resize(num_leds);
}

void HueDevice::SetLEDs(const std::vector<Color>& colors) {
    // Non-blocking: quickly copy colors to a shared_ptr and publish
    auto buf = std::make_shared<std::vector<Color>>(colors);
    latest_slot_.store(buf);
}

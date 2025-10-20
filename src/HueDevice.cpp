#include "HueDevice.h"
#include <memory>

HueDevice::HueDevice(int num_leds) {
    // In a real implementation, you would set up the device name, type, zones, LEDs etc.
    // For this plugin, we just need a virtual device to receive color data.
}

void HueDevice::SetLEDs(const std::vector<Color>& colors) {
    // Non-blocking: quickly copy colors to a shared_ptr and publish to the slot.
    auto buf = std::make_shared<std::vector<Color>>(colors);
    latest_slot_.store(buf);
}

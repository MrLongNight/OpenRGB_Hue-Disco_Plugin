#include "HueDevice.h"
#include "HueStreamer.h"
#include <memory>

HueDevice::HueDevice(HueStreamer* streamer, int num_leds) : streamer_(streamer) {
    // Basic device setup
    device_name_ = "Philips Hue Entertainment";
    device_type_ = "Entertainment Area";
    vendor_ = "Signify";

    // Create a single zone for all lights in the entertainment area
    zones_.resize(1);
    zones_[0].leds_.resize(num_leds);
}

void HueDevice::SetLEDs(const std::vector<Color>& colors) {
    if (streamer_) {
        // Non-blocking: quickly copy colors to a shared_ptr and publish to the slot.
        auto buf = std::make_shared<std::vector<Color>>(colors);
        streamer_->get_latest_slot()->store(buf);
    }
}

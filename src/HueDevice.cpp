#include "HueDevice.h"
#include "HueStreamer.h"
#include <memory>

HueDevice::HueDevice(HueStreamer* streamer, int num_leds) : streamer_(streamer) {
    device_name_ = "Philips Hue Entertainment";
    device_type_ = "Entertainment Area";
    vendor_ = "Signify";

    zones_.resize(1);
    zones_[0].leds_.resize(num_leds);
}

void HueDevice::SetLEDs(const std::vector<Color>& colors) {
    if (streamer_) {
        auto buf = std::make_shared<std::vector<Color>>(colors);
        streamer_->get_latest_slot()->store(buf);
        streamer_->notify_new_frame();
    }
}

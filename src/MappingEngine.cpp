#include "MappingEngine.h"

MappingEngine::MappingEngine(const std::vector<std::string>& lamp_uuids) {
    lamp_uuids_ = lamp_uuids;
    std::sort(lamp_uuids_.begin(), lamp_uuids_.end());
}

void MappingEngine::mapOpenRGBtoHue(const std::vector<Color>& openrgb_leds, std::vector<MappedHueColor>& hue_frame) {
    size_t N_leds = openrgb_leds.size();
    size_t N_lamps = lamp_uuids_.size();
    hue_frame.clear();
    if (N_leds == 0 || N_lamps == 0) return;

    if (N_leds == N_lamps) {
        map_1to1(openrgb_leds, hue_frame);
    } else if (N_leds > N_lamps) {
        map_segment_average(openrgb_leds, hue_frame);
    } else {
        map_interpolate(openrgb_leds, hue_frame);
    }
}

void MappingEngine::color_to_float(const Color& in, float& r, float& g, float& b) {
    r = in.r / 255.0f;
    g = in.g / 255.0f;
    b = in.b / 255.0f;
}

void MappingEngine::map_1to1(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame) {
    for (size_t i = 0; i < lamp_uuids_.size(); ++i) {
        float r, g, b;
        color_to_float(leds[i], r, g, b);
        frame.push_back(MappedHueColor{r, g, b, lamp_uuids_[i]});
    }
}

void MappingEngine::map_segment_average(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame) {
    size_t N_leds = leds.size();
    size_t N_lamps = lamp_uuids_.size();
    if (N_lamps == 0) return;

    float seg_size = static_cast<float>(N_leds) / static_cast<float>(N_lamps);
    for (size_t i = 0; i < N_lamps; ++i) {
        size_t start = static_cast<size_t>(std::floor(i * seg_size));
        size_t end = static_cast<size_t>(std::floor((i + 1) * seg_size));
        end = std::min(end, N_leds);
        if (start >= end) end = std::min(start + 1, N_leds);

        float r_sum = 0.0f, g_sum = 0.0f, b_sum = 0.0f;
        size_t count = end - start;

        if (count == 0) {
            if (!leds.empty()) {
                float r, g, b;
                color_to_float(leds[start], r, g, b);
                frame.push_back(MappedHueColor{r, g, b, lamp_uuids_[i]});
            }
            continue;
        }

        for (size_t j = start; j < end; ++j) {
            r_sum += leds[j].r / 255.0f;
            g_sum += leds[j].g / 255.0f;
            b_sum += leds[j].b / 255.0f;
        }

        frame.push_back(MappedHueColor{r_sum / count, g_sum / count, b_sum / count, lamp_uuids_[i]});
    }
}

void MappingEngine::map_interpolate(const std::vector<Color>& leds, std::vector<MappedHueColor>& frame) {
    size_t N_leds = leds.size();
    size_t N_lamps = lamp_uuids_.size();
    if (N_leds == 0) return;

    for (size_t i = 0; i < N_lamps; ++i) {
        float pos = (static_cast<float>(i) + 0.5f) * (static_cast<float>(N_leds) / static_cast<float>(N_lamps));
        size_t idx = static_cast<size_t>(std::floor(pos));
        idx = std::min(idx, N_leds - 1);
        float r, g, b;
        color_to_float(leds[idx], r, g, b);
        frame.push_back(MappedHueColor{r, g, b, lamp_uuids_[i]});
    }
}

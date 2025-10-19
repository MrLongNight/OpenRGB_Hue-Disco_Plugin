#include "MappingEngine.h"
#include <numeric>
#include <cmath>

void MappingEngine::loadMapping(const std::vector<Mapping>& mappings) {
    mappings_ = mappings;
}

void MappingEngine::mapOpenRGBtoHue(const std::vector<Color>& in, std::vector<HueColor>& out) {
    if (mappings_.empty()) {
        // Default mapping logic
        if (in.empty() || out.empty()) {
            out.clear();
            return;
        }

        if (in.size() > out.size()) {
            // Downsample (average)
            for (size_t i = 0; i < out.size(); ++i) {
                size_t start = i * in.size() / out.size();
                size_t end = (i + 1) * in.size() / out.size();
                unsigned int r = 0, g = 0, b = 0;
                for (size_t j = start; j < end; ++j) {
                    r += in[j].r;
                    g += in[j].g;
                    b += in[j].b;
                }
                out[i] = {
                    static_cast<uint8_t>(r / (end - start)),
                    static_cast<uint8_t>(g / (end - start)),
                    static_cast<uint8_t>(b / (end - start))
                };
            }
        } else if (in.size() < out.size()) {
            // Upsample (interpolate)
             for (size_t i = 0; i < out.size(); ++i) {
                float in_index = (float)i * (in.size() - 1) / (out.size() - 1);
                int index1 = floor(in_index);
                int index2 = ceil(in_index);
                float fraction = in_index - index1;

                if (index1 < 0) index1 = 0;
                if (index2 >= in.size()) index2 = in.size() - 1;


                if (index1 == index2) {
                    out[i] = {in[index1].r, in[index1].g, in[index1].b};
                } else {
                    uint8_t r = in[index1].r * (1 - fraction) + in[index2].r * fraction;
                    uint8_t g = in[index1].g * (1 - fraction) + in[index2].g * fraction;
                    uint8_t b = in[index1].b * (1 - fraction) + in[index2].b * fraction;
                    out[i] = {r, g, b};
                }
            }
        } else {
            // 1:1 mapping
            for (size_t i = 0; i < in.size(); ++i) {
                out[i] = {in[i].r, in[i].g, in[i].b};
            }
        }
        return;
    }

    out.resize(mappings_.size());
    for (size_t i = 0; i < mappings_.size(); ++i) {
        const auto& mapping = mappings_[i];
        if (mapping.openrgb_indices.empty() || in.empty()) continue;

        unsigned int r = 0, g = 0, b = 0;
        for (int index : mapping.openrgb_indices) {
            if (index < in.size()) {
                r += in[index].r;
                g += in[index].g;
                b += in[index].b;
            }
        }
        
        out[i] = {
            static_cast<uint8_t>(r / mapping.openrgb_indices.size()),
            static_cast<uint8_t>(g / mapping.openrgb_indices.size()),
            static_cast<uint8_t>(b / mapping.openrgb_indices.size())
        };
    }
}

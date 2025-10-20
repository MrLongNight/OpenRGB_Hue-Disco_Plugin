#ifndef OPENRGB_CONFIG_READER_H
#define OPENRGB_CONFIG_READER_H

#include <string>
#include <vector>
#include <optional>
#include "nlohmann/json.hpp"

struct HueBridge {
    std::string ip;
    std::string username;
    std::string clientkey;
    std::string entertainment_area_id;
};

class OpenRGBConfigReader {
public:
    OpenRGBConfigReader();
    std::vector<HueBridge> get_hue_bridges() const;

private:
    std::optional<std::string> get_config_path() const;
    nlohmann::json config_data;
};

#endif // OPENRGB_CONFIG_READER_H

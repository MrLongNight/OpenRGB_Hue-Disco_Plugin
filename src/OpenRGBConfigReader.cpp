#include "OpenRGBConfigReader.h"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <filesystem>

#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#endif

OpenRGBConfigReader::OpenRGBConfigReader() {
    auto path = get_config_path();
    if (path) {
        std::ifstream file(*path);
        if (file.is_open()) {
            try {
                config_data = nlohmann::json::parse(file);
            } catch (const nlohmann::json::parse_error& e) {
                std::cerr << "Failed to parse openrgb.json: " << e.what() << std::endl;
            }
        }
    }
}

std::vector<HueBridge> OpenRGBConfigReader::get_hue_bridges() const {
    std::vector<HueBridge> bridges;
    if (config_data.contains("detectors")) {
        for (const auto& detector : config_data["detectors"]) {
            if (detector.contains("type") && detector["type"] == "PhilipsHueDetector") {
                if (detector.contains("bridges")) {
                    for (const auto& bridge_config : detector["bridges"]) {
                        if (bridge_config.contains("entertainment") && bridge_config["entertainment"].get<bool>()) {
                            HueBridge bridge;
                            bridge.ip = bridge_config.value("ip", "");
                            bridge.username = bridge_config.value("username", "");
                            bridge.clientkey = bridge_config.value("clientkey", "");
                            bridge.entertainment_area_id = bridge_config.value("entertainment_area", "");
                            bridges.push_back(bridge);
                        }
                    }
                }
            }
        }
    }
    return bridges;
}

std::optional<std::string> OpenRGBConfigReader::get_config_path() const {
    std::filesystem::path config_path;
#ifdef _WIN32
    PWSTR path_tmp;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path_tmp))) {
        config_path = path_tmp;
        CoTaskMemFree(path_tmp);
        config_path /= "OpenRGB";
    }
#else
    const char* home_dir = getenv("HOME");
    if (home_dir) {
        config_path = home_dir;
        config_path /= ".config/OpenRGB";
    }
#endif
    if (!config_path.empty()) {
        config_path /= "openrgb.json";
        if (std::filesystem::exists(config_path)) {
            return config_path.string();
        }
    }
    return std::nullopt;
}

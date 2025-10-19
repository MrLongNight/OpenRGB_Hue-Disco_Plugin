#include "ConfigManager.h"
#include "Logger.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

ConfigManager::ConfigManager(const std::string& config_path) : config_path_(config_path) {}

bool ConfigManager::load() {
    try {
        if (!fs::exists(config_path_)) {
            spdlog::warn("Config file not found at {}. Creating a default one.", config_path_);
            config_data_ = {
                {"bridge_ip", ""},
                {"username", ""},
                {"clientkey", ""},
                {"area_id", ""},
                {"mappings", json::array()},
                {"target_fps", 30}
            };
            return save();
        }

        std::ifstream file(config_path_);
        file >> config_data_;
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to load config: {}", e.what());
        return false;
    }
}

bool ConfigManager::save() {
    try {
        fs::path config_dir = fs::path(config_path_).parent_path();
        if (!fs::exists(config_dir)) {
            fs::create_directories(config_dir);
        }
        std::ofstream file(config_path_);
        file << config_data_.dump(4);
        // Set file permissions (user only)
        fs::permissions(config_path_, fs::perms::owner_read | fs::perms::owner_write, fs::perm_options::replace);
        return true;
    } catch (const std::exception& e) {
        spdlog::error("Failed to save config: {}", e.what());
        return false;
    }
}

std::string ConfigManager::get_bridge_ip() const {
    return config_data_.value("bridge_ip", "");
}

void ConfigManager::set_bridge_ip(const std::string& ip) {
    config_data_["bridge_ip"] = ip;
}

std::string ConfigManager::get_username() const {
    return config_data_.value("username", "");
}

void ConfigManager::set_username(const std::string& username) {
    config_data_["username"] = username;
}

std::string ConfigManager::get_clientkey() const {
    return config_data_.value("clientkey", "");
}

void ConfigManager::set_clientkey(const std::string& clientkey) {
    config_data_["clientkey"] = clientkey;
}

std::string ConfigManager::get_area_id() const {
    return config_data_.value("area_id", "");
}

void ConfigManager::set_area_id(const std::string& area_id) {
    config_data_["area_id"] = area_id;
}

std::vector<Mapping> ConfigManager::get_mappings() const {
    std::vector<Mapping> mappings;
    if (config_data_.contains("mappings")) {
        for (const auto& m : config_data_["mappings"]) {
            mappings.push_back({m["openrgb_indices"].get<std::vector<int>>(), m["lamp_id"].get<std::string>()});
        }
    }
    return mappings;
}

void ConfigManager::set_mappings(const std::vector<Mapping>& mappings) {
    json mappings_json = json::array();
    for (const auto& m : mappings) {
        mappings_json.push_back({
            {"openrgb_indices", m.openrgb_indices},
            {"lamp_id", m.lamp_id}
        });
    }
    config_data_["mappings"] = mappings_json;
}

int ConfigManager::get_target_fps() const {
    return config_data_.value("target_fps", 30);
}

void ConfigManager::set_target_fps(int fps) {
    config_data_["target_fps"] = fps;
}

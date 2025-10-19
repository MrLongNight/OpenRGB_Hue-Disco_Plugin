#pragma once

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct Mapping {
    std::vector<int> openrgb_indices;
    std::string lamp_id;
};

class ConfigManager {
public:
    ConfigManager(const std::string& config_path);
    bool load();
    bool save();

    std::string get_bridge_ip() const;
    void set_bridge_ip(const std::string& ip);

    std::string get_username() const;
    void set_username(const std::string& username);

    std::string get_clientkey() const;
    void set_clientkey(const std::string& clientkey);
    
    std::string get_area_id() const;
    void set_area_id(const std::string& area_id);

    std::vector<Mapping> get_mappings() const;
    void set_mappings(const std::vector<Mapping>& mappings);

    int get_target_fps() const;
    void set_target_fps(int fps);

private:
    std::string config_path_;
    json config_data_;
};

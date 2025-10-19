#include "PluginHueEntertainment.h"
#include "Logger.h"
#include <filesystem>

// This function is the entry point for the plugin
extern "C" Plugin* create_plugin() {
    return new PluginHueEntertainment();
}

void PluginHueEntertainment::init() {
    InitializeLogger();
    spdlog::info("Initializing OpenRGB Hue Entertainment Plugin");

    // Determine config path
    std::string config_path;
#ifdef _WIN32
    config_path = std::string(getenv("APPDATA")) + "/openrgb-hue-entertainment/config.json";
#else
    config_path = std::string(getenv("HOME")) + "/.config/openrgb-hue-entertainment/config.json";
#endif

    config_manager_ = std::make_shared<ConfigManager>(config_path);
    if (!config_manager_->load()) {
        spdlog::error("Failed to load configuration. Plugin will not start.");
        return;
    }

    // Check if we need to run pushlink
    if (config_manager_->get_username().empty() || config_manager_->get_clientkey().empty()) {
        spdlog::info("Username or clientkey not found. Starting pushlink registration.");
        std::string username, clientkey;
        HueClient client(config_manager_->get_bridge_ip(), "");
        if (client.registerUserWithPushlink(username, clientkey)) {
            config_manager_->set_username(username);
            config_manager_->set_clientkey(clientkey);
            config_manager_->save();
        } else {
            spdlog::error("Pushlink registration failed. Plugin will not start.");
            return;
        }
    }

    hue_client_ = std::make_shared<HueClient>(config_manager_->get_bridge_ip(), config_manager_->get_username());
    
    auto areas = hue_client_->getEntertainmentAreas();
    if (areas.empty()) {
        spdlog::error("No entertainment areas found. Plugin will not start.");
        return;
    }

    // For now, just use the first area
    auto& area = areas[0];
    config_manager_->set_area_id(area.id);
    
    // Create mappings if they don't exist
    if (config_manager_->get_mappings().empty()) {
        std::vector<Mapping> mappings;
        for (int i = 0; i < area.lamp_uuids.size(); ++i) {
            mappings.push_back({{i}, area.lamp_uuids[i]});
        }
        config_manager_->set_mappings(mappings);
        config_manager_->save();
    }


    dtls_client_ = std::make_shared<DTLSClient>(config_manager_->get_bridge_ip(), config_manager_->get_username(), config_manager_->get_clientkey(), config_manager_->get_area_id(), config_manager_->get_mappings());
    mapping_engine_ = std::make_shared<MappingEngine>(area.lamp_uuids);

    hue_streamer_ = std::make_shared<HueStreamer>(config_manager_, dtls_client_, mapping_engine_);
    
    hue_device_ = std::make_unique<HueDevice>(hue_streamer_, area.lamp_uuids.size());

    // Register device with OpenRGB
    register_device(hue_device_.get());

    hue_streamer_->start();
}

void PluginHueEntertainment::cleanup() {
    spdlog::info("Cleaning up OpenRGB Hue Entertainment Plugin");
    if (hue_streamer_) {
        hue_streamer_->stop();
    }
}

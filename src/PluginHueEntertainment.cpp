#include "PluginHueEntertainment.h"
#include "Logger.h"
#include <filesystem>
#include <thread>

extern "C" Plugin* create_plugin() {
    return new PluginHueEntertainment();
}

void PluginHueEntertainment::init() {
    InitializeLogger();
    spdlog::info("Initializing OpenRGB Hue Entertainment Plugin");

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

    std::string bridge_ip = config_manager_->get_bridge_ip();
    if (bridge_ip.empty()) {
        HueClient client("", "");
        if (client.discoverBridge(bridge_ip)) {
            config_manager_->set_bridge_ip(bridge_ip);
            config_manager_->save();
        } else {
            spdlog::error("Could not discover Hue Bridge. Please set bridge_ip in config.");
#ifdef WITH_QT_WIDGETS
            if(widget_) widget_->set_status_text("Bridge not found. Please set IP in config.");
#endif
            return;
        }
    }

#ifdef WITH_QT_WIDGETS
    if (widget_) {
        widget_->set_bridge_ip(config_manager_->get_bridge_ip());
    }
#endif

    if (config_manager_->get_username().empty() || config_manager_->get_clientkey().empty()) {
        spdlog::warn("Username or clientkey not found. Please authenticate via the plugin UI.");
#ifdef WITH_QT_WIDGETS
        if(widget_) widget_->set_status_text("Not authenticated. Please press the button.");
#endif
        return; // Stop initialization here, wait for user action
    }

    // --- All further initialization only happens if we are authenticated ---

    hue_client_ = std::make_shared<HueClient>(config_manager_->get_bridge_ip(), config_manager_->get_username());
    
    auto areas = hue_client_->getEntertainmentAreas();
    if (areas.empty()) {
        spdlog::error("No entertainment areas found on the Hue Bridge.");
        return;
    }

    auto& area = areas[0];
    config_manager_->set_area_id(area.id);
    
    if (config_manager_->get_mappings().empty()) {
        std::vector<Mapping> mappings;
        for (size_t i = 0; i < area.lamp_uuids.size(); ++i) {
            mappings.push_back({{ (int)i }, area.lamp_uuids[i]});
        }
        config_manager_->set_mappings(mappings);
        config_manager_->save();
    }

    dtls_client_ = std::make_shared<DTLSClient>(config_manager_->get_bridge_ip(), config_manager_->get_username(), config_manager_->get_clientkey(), config_manager_->get_area_id(), config_manager_->get_mappings());
    mapping_engine_ = std::make_shared<MappingEngine>(area.lamp_uuids);
    hue_device_ = std::make_unique<HueDevice>(area.lamp_uuids.size());
    hue_streamer_ = std::make_shared<HueStreamer>(config_manager_, dtls_client_, mapping_engine_, &hue_device_->latest_slot_);

    register_device(hue_device_.get());
    hue_streamer_->start();

#ifdef WITH_QT_WIDGETS
    if(widget_) widget_->set_status_text("Connected and streaming!");
#endif
}

void PluginHueEntertainment::start_pushlink_authentication() {
    // Run in a separate thread to not block the UI
    std::thread([this]() {
        spdlog::info("Starting pushlink registration from UI.");
        std::string username, clientkey;
        HueClient client(config_manager_->get_bridge_ip(), "");

        if (client.registerUserWithPushlink(username, clientkey)) {
            config_manager_->set_username(username);
            config_manager_->set_clientkey(clientkey);
            config_manager_->save();
            spdlog::info("Successfully registered! Restarting plugin logic...");
#ifdef WITH_QT_WIDGETS
            if(widget_) widget_->set_status_text("Success! Restarting...");
#endif
            // Re-run init to start the streaming with the new credentials
            init();
        } else {
            spdlog::error("Pushlink registration failed.");
#ifdef WITH_QT_WIDGETS
            if(widget_) widget_->set_status_text("Authentication failed. Please try again.");
#endif
        }
    }).detach();
}

#ifdef WITH_QT_WIDGETS
QWidget* PluginHueEntertainment::GetWidget() {
    if (widget_ == nullptr) {
        widget_ = new HuePluginWidget(this);
        if (config_manager_) {
            widget_->set_bridge_ip(config_manager_->get_bridge_ip());
            if (config_manager_->get_username().empty()) {
                widget_->set_status_text("Ready to authenticate.");
            }
        }
    }
    return widget_;
}
#endif

void PluginHueEntertainment::cleanup() {
    spdlog::info("Cleaning up OpenRGB Hue Entertainment Plugin");
    if (hue_streamer_) {
        hue_streamer_->stop();
    }
}

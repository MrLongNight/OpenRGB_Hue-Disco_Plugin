#include "PluginHueEntertainment.h"
#include "Logger.h"

static PluginHueEntertainment* plugin_instance = nullptr;

extern "C" void init_plugin(plugin_api_t* api) {
    plugin_instance = new PluginHueEntertainment();
    plugin_instance->delayed_init(api);
}

extern "C" Plugin* get_plugin_instance() {
    return plugin_instance;
}

void PluginHueEntertainment::delayed_init(plugin_api_t* api) {
    plugin_api_ = api;
    InitializeLogger();
    spdlog::info("Initializing OpenRGB Hue Entertainment Plugin");

    config_reader_ = std::make_unique<OpenRGBConfigReader>();
    available_bridges_ = config_reader_->get_hue_bridges();

    if (available_bridges_.empty()) {
        current_state_ = State::NO_BRIDGES_FOUND;
        spdlog::warn("No Philips Hue bridges with Entertainment enabled were found in your OpenRGB config.");
    } else {
        current_state_ = State::SELECT_BRIDGE;
        spdlog::info("Found {} configured Hue Entertainment bridge(s).", available_bridges_.size());
    }
}

void PluginHueEntertainment::cleanup() {
    spdlog::info("Cleaning up OpenRGB Hue Entertainment Plugin");
    if (hue_streamer_) {
        hue_streamer_->stop();
    }
    if (hue_device_) {
        plugin_api_->unregister_device(hue_device_.get());
    }
    delete plugin_instance;
}

void PluginHueEntertainment::create_widgets(int parent_widget_id) {
    main_widget_id_ = plugin_api_->add_widget(parent_widget_id, "group_box", {{"label", "Hue Entertainment"}});
    status_label_id_ = plugin_api_->add_widget(main_widget_id_, "label", {{"label", "Initializing..."}});
    bridge_selection_id_ = plugin_api_->add_widget(main_widget_id_, "combo_box", {});
    start_button_id_ = plugin_api_->add_widget(main_widget_id_, "button", {{"label", "Start Streaming"}});

    update_ui();

    plugin_api_->register_widget_callback(start_button_id_, [this](const std::map<std::string, p_val>&) {
        if (current_state_ == State::READY) {
            auto selected_text = plugin_api_->get_widget_value(bridge_selection_id_, "text").s_val;
            for(const auto& bridge : available_bridges_) {
                if (bridge.ip == selected_text) {
                     start_streaming(bridge);
                     break;
                }
            }
        }
    });
}

void PluginHueEntertainment::update_ui() {
    switch (current_state_) {
        case State::NO_BRIDGES_FOUND:
            plugin_api_->set_widget_property(status_label_id_, "label", "Error: No Hue Bridges with Entertainment enabled in OpenRGB settings.\nPlease configure a Hue Bridge in the main OpenRGB settings and enable Entertainment mode for it.");
            plugin_api_->set_widget_property(bridge_selection_id_, "visible", false);
            plugin_api_->set_widget_property(start_button_id_, "visible", false);
            break;
        case State::SELECT_BRIDGE:
            plugin_api_->set_widget_property(status_label_id_, "label", "Select a Hue Bridge to start:");
            {
                std::string bridge_ips_str;
                for (const auto& bridge : available_bridges_) {
                    bridge_ips_str += bridge.ip + ",";
                }
                if (!bridge_ips_str.empty()) {
                    bridge_ips_str.pop_back(); // Remove trailing comma
                }
                plugin_api_->set_widget_property(bridge_selection_id_, "items", bridge_ips_str.c_str());
            }
            plugin_api_->set_widget_property(bridge_selection_id_, "visible", true);
            plugin_api_->set_widget_property(start_button_id_, "visible", true);
            current_state_ = State::READY;
            break;
        case State::STREAMING:
             plugin_api_->set_widget_property(status_label_id_, "label", "Streaming active.");
             plugin_api_->set_widget_property(bridge_selection_id_, "visible", false);
             plugin_api_->set_widget_property(start_button_id_, "visible", false);
            break;
        case State::ERROR:
             plugin_api_->set_widget_property(status_label_id_, "label", "An error occurred. Check logs.");
             plugin_api_->set_widget_property(bridge_selection_id_, "visible", false);
             plugin_api_->set_widget_property(start_button_id_, "visible", false);
             break;

    }
}

void PluginHueEntertainment::start_streaming(const HueBridge& bridge) {
    spdlog::info("Starting stream for bridge: {}", bridge.ip);

    hue_client_ = std::make_shared<HueClient>(bridge.ip);
    auto areas = hue_client_->getEntertainmentAreas(bridge.username);
    HueEntertainmentArea* selected_area = nullptr;
    for(auto& area : areas) {
        if(area.id == bridge.entertainment_area_id) {
            selected_area = &area;
            break;
        }
    }

    if(!selected_area) {
        spdlog::error("Could not find configured entertainment area on bridge. Aborting.");
        current_state_ = State::ERROR;
        update_ui();
        return;
    }

    current_state_ = State::STREAMING;
    update_ui();

    mapping_engine_ = std::make_shared<MappingEngine>(selected_area->lamp_uuids);
    dtls_client_ = std::make_shared<DTLSClient>(bridge.ip, bridge.username, bridge.clientkey, bridge.entertainment_area_id, std::vector<Mapping>{});
    hue_streamer_ = std::make_shared<HueStreamer>(dtls_client_, mapping_engine_);
    hue_device_ = std::make_unique<HueDevice>(hue_streamer_.get(), selected_area->lamp_uuids.size());

    plugin_api_->register_device(hue_device_.get());
    hue_streamer_->start();
}

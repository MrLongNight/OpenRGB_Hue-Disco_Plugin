#pragma once

#include "OpenRGB/Plugin.h"
#include "OpenRGB/PluginAPI.h"
#include "OpenRGBConfigReader.h"
#include "HueClient.h"
#include "DTLSClient.h"
#include "MappingEngine.h"
#include "HueStreamer.h"
#include "HueDevice.h"
#include <memory>
#include <vector>
#include <map>

class PluginHueEntertainment : public Plugin {
public:
    enum class State {
        NO_CONFIG_FILE,
        NO_BRIDGES_FOUND,
        BRIDGE_NOT_CONFIGURED_FOR_ENTERTAINMENT,
        SELECT_BRIDGE,
        READY,
        STREAMING,
        ERROR
    };

    void init() override {} // Dummy impl to satisfy abstract base
    void delayed_init(plugin_api_t* api);
    void cleanup() override;
    void create_widgets(int parent_widget_id);

private:
    void update_ui();
    void start_streaming(const HueBridge& bridge);

    plugin_api_t* plugin_api_ = nullptr;
    std::unique_ptr<OpenRGBConfigReader> config_reader_;
    std::vector<HueBridge> available_bridges_;
    State current_state_ = State::NO_CONFIG_FILE;

    std::shared_ptr<HueClient> hue_client_;
    std::shared_ptr<DTLSClient> dtls_client_;
    std::shared_ptr<MappingEngine> mapping_engine_;
    std::shared_ptr<HueStreamer> hue_streamer_;
    std::unique_ptr<HueDevice> hue_device_;

    // UI widget IDs
    int main_widget_id_ = -1;
    int status_label_id_ = -1;
    int bridge_selection_id_ = -1;
    int start_button_id_ = -1;
};

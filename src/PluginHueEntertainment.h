#pragma once

#include "OpenRGB/Plugin.h"
#include "ConfigManager.h"
#include "HueClient.h"
#include "DTLSClient.h"
#include "MappingEngine.h"
#include "HueStreamer.h"
#include "HueDevice.h"
#include <memory>

#ifdef WITH_QT_WIDGETS
#include "HuePluginWidget.h"
#endif

class PluginHueEntertainment : public Plugin {
public:
    void init() override;
    void cleanup() override;
    void start_pushlink_authentication();

#ifdef WITH_QT_WIDGETS
    QWidget* GetWidget() override;
#endif

private:
#ifdef WITH_QT_WIDGETS
    HuePluginWidget* widget_ = nullptr;
#endif
    std::shared_ptr<ConfigManager> config_manager_;
    std::shared_ptr<HueClient> hue_client_;
    std::shared_ptr<DTLSClient> dtls_client_;
    std::shared_ptr<MappingEngine> mapping_engine_;
    std::shared_ptr<HueStreamer> hue_streamer_;
    std::unique_ptr<HueDevice> hue_device_;
};

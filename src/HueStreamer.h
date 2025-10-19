#pragma once

#include "DTLSClient.h"
#include "MappingEngine.h"
#include "ConfigManager.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>

class HueStreamer {
public:
    HueStreamer(std::shared_ptr<ConfigManager> config, std::shared_ptr<DTLSClient> dtls_client, std::shared_ptr<MappingEngine> mapping_engine);
    ~HueStreamer();

    void start();
    void stop();
    void updateColors(const std::vector<Color>& colors);

private:
    void stream_thread_func();

    std::shared_ptr<ConfigManager> config_;
    std::shared_ptr<DTLSClient> dtls_client_;
    std::shared_ptr<MappingEngine> mapping_engine_;

    std::thread stream_thread_;
    std::atomic<bool> running_{false};

    // Lock-free slot for latest frame
    std::shared_ptr<std::vector<Color>> latest_frame_{nullptr};
    std::mutex frame_mutex_;

    uint32_t sequence_number_ = 0;
};

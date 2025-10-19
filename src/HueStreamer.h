#pragma once

#include "DTLSClient.h"
#include "MappingEngine.h"
#include "ConfigManager.h"
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

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

    std::vector<Color> color_buffer_;
    std::mutex buffer_mutex_;
    std::condition_variable buffer_cv_;
    std::atomic<bool> new_frame_ready_{false};
    
    uint32_t sequence_number_ = 0;
};

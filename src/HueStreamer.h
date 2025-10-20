#pragma once

#include "DTLSClient.h"
#include "MappingEngine.h"
#include "ConfigManager.h"
#include "LatestFrame.h"
#include <vector>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

class HueStreamer {
public:
    HueStreamer(std::shared_ptr<ConfigManager> config, std::shared_ptr<DTLSClient> dtls_client, std::shared_ptr<MappingEngine> mapping_engine, LatestSlot<std::vector<Color>>* latest_slot);
    ~HueStreamer();

    void start();
    void stop();

private:
    void stream_thread_func();

    std::shared_ptr<ConfigManager> config_;
    std::shared_ptr<DTLSClient> dtls_client_;
    std::shared_ptr<MappingEngine> mapping_engine_;
    LatestSlot<std::vector<Color>>* latest_slot_;

    std::thread stream_thread_;
    std::atomic<bool> running_{false};

    std::mutex main_mutex_;
    std::condition_variable new_frame_cv_;

    uint32_t sequence_number_ = 0;
};

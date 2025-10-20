#include "HueStreamer.h"
#include "Logger.h"
#include <chrono>
#include <algorithm>
#include <spdlog/spdlog.h>

HueStreamer::HueStreamer(std::shared_ptr<DTLSClient> dtls_client, std::shared_ptr<MappingEngine> mapping_engine)
    : dtls_client_(dtls_client), mapping_engine_(mapping_engine) {
    latest_slot_ = std::make_unique<LatestSlot<std::vector<Color>>>();
}

HueStreamer::~HueStreamer() {
    stop();
}

void HueStreamer::start() {
    if (running_.load(std::memory_order_acquire)) return;
    running_.store(true, std::memory_order_release);
    stream_thread_ = std::thread(&HueStreamer::stream_thread_func, this);
}

void HueStreamer::stop() {
    if (!running_.load(std::memory_order_acquire)) return;
    running_.store(false, std::memory_order_release);
    new_frame_cv_.notify_one();
    if (stream_thread_.joinable()) {
        stream_thread_.join();
    }
}

void HueStreamer::stream_thread_func() {
    int target_fps = 30;
    auto frame_duration = std::chrono::milliseconds(1000 / target_fps);
    long backoff_ms = 500;

    std::shared_ptr<std::vector<Color>> current_frame = nullptr;
    std::shared_ptr<std::vector<Color>> last_processed_frame = nullptr;

    while (running_.load(std::memory_order_acquire)) {
        if (!dtls_client_->isConnected()) {
            if (!dtls_client_->connect()) {
                spdlog::error("DTLS connection failed. Retrying in {} ms", backoff_ms);
                std::this_thread::sleep_for(std::chrono::milliseconds(backoff_ms));
                backoff_ms = std::min(backoff_ms * 2, 30000L);
                continue;
            }
            backoff_ms = 500;
            spdlog::info("DTLS connected");
        }

        auto loop_start = std::chrono::steady_clock::now();

        current_frame = latest_slot_->exchange(nullptr);

        if (!current_frame) {
             current_frame = last_processed_frame;
        }

        if (current_frame) {
            std::vector<MappedHueColor> mapped_colors;
            mapping_engine_->mapOpenRGBtoHue(*current_frame, mapped_colors);

            std::vector<DTLSHueColor> dtls_colors;
            std::vector<std::string> lamp_ids;
            for(const auto& color : mapped_colors) {
                dtls_colors.push_back({(uint8_t)(color.r * 255.0f), (uint8_t)(color.g * 255.0f), (uint8_t)(color.b * 255.0f)});
                lamp_ids.push_back(color.lamp_uuid);
            }

            if (!dtls_client_->sendFrame(dtls_colors, lamp_ids, ++sequence_number_)) {
                spdlog::error("sendFrame failed, will disconnect and reconnect");
                dtls_client_->disconnect();
            }
            last_processed_frame = current_frame;
        }

        auto loop_end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);

        if (elapsed < frame_duration) {
             std::this_thread::sleep_for(frame_duration - elapsed);
        }
    }
}

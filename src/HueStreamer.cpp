#include "HueStreamer.h"
#include "Logger.h"
#include <chrono>
#include <algorithm>
#include <spdlog/spdlog.h>

HueStreamer::HueStreamer(std::shared_ptr<ConfigManager> config, std::shared_ptr<DTLSClient> dtls_client, std::shared_ptr<MappingEngine> mapping_engine)
    : config_(config), dtls_client_(dtls_client), mapping_engine_(mapping_engine) {}

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
    if (stream_thread_.joinable()) {
        stream_thread_.join();
    }
}

void HueStreamer::updateColors(const std::vector<Color>& colors) {
    std::lock_guard<std::mutex> lock(frame_mutex_);
    latest_frame_ = std::make_shared<std::vector<Color>>(colors);
}

void HueStreamer::stream_thread_func() {
    int target_fps = config_->get_target_fps();
    if (target_fps <= 0) target_fps = 30;
    auto frame_duration = std::chrono::milliseconds(1000 / target_fps);
    long backoff_ms = 500;

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

        std::shared_ptr<std::vector<Color>> frame_to_process;
        {
            std::lock_guard<std::mutex> lock(frame_mutex_);
            if (latest_frame_ != last_processed_frame) {
                frame_to_process = latest_frame_;
                last_processed_frame = latest_frame_;
            }
        }

        if (frame_to_process) {
            // Map OpenRGB colors -> Hue colors (floats 0.0-1.0)
            std::vector<MappedHueColor> mapped_colors;
            mapping_engine_->mapOpenRGBtoHue(*frame_to_process, mapped_colors);

            std::vector<DTLSHueColor> dtls_colors;
            for(const auto& color : mapped_colors) {
                dtls_colors.push_back({(uint8_t)(color.r * 255), (uint8_t)(color.g * 255), (uint8_t)(color.b * 255)});
            }

            // Send via DTLS; non-blocking wrt SetLEDs (DTLS handled in streamer thread)
            if (!dtls_client_->sendFrame(dtls_colors, ++sequence_number_)) {
                spdlog::error("sendFrame failed, will disconnect and reconnect");
                dtls_client_->disconnect();
            }
        }

        auto loop_end = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(loop_end - loop_start);
        if (elapsed < frame_duration) {
            std::this_thread::sleep_for(frame_duration - elapsed);
        }
    }
}
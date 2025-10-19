#include "HueStreamer.h"
#include "Logger.h"

HueStreamer::HueStreamer(std::shared_ptr<ConfigManager> config, std::shared_ptr<DTLSClient> dtls_client, std::shared_ptr<MappingEngine> mapping_engine)
    : config_(config), dtls_client_(dtls_client), mapping_engine_(mapping_engine) {}

HueStreamer::~HueStreamer() {
    stop();
}

void HueStreamer::start() {
    if (running_) return;
    running_ = true;
    stream_thread_ = std::thread(&HueStreamer::stream_thread_func, this);
}

void HueStreamer::stop() {
    if (!running_) return;
    running_ = false;
    buffer_cv_.notify_one();
    if (stream_thread_.joinable()) {
        stream_thread_.join();
    }
}

void HueStreamer::updateColors(const std::vector<Color>& colors) {
    {
        std::lock_guard<std::mutex> lock(buffer_mutex_);
        color_buffer_ = colors;
    }
    new_frame_ready_ = true;
    buffer_cv_.notify_one();
}

void HueStreamer::stream_thread_func() {
    int target_fps = config_->get_target_fps();
    auto frame_duration = std::chrono::milliseconds(1000 / target_fps);
    long backoff_ms = 500;

    while (running_) {
        if (!dtls_client_->connect()) {
            spdlog::error("DTLS connection failed. Retrying in {}ms", backoff_ms);
            std::this_thread::sleep_for(std::chrono::milliseconds(backoff_ms));
            backoff_ms = std::min(backoff_ms * 2, 30000L); // Exponential backoff up to 30s
            continue;
        }
        
        backoff_ms = 500; // Reset backoff on successful connect
        spdlog::info("DTLS connection established. Starting stream.");

        while(running_) {
            auto start_time = std::chrono::steady_clock::now();
            
            {
                std::unique_lock<std::mutex> lock(buffer_mutex_);
                buffer_cv_.wait(lock, [this]{ return new_frame_ready_ || !running_; });

                if (!running_) break;

                std::vector<HueColor> hue_colors;
                mapping_engine_->mapOpenRGBtoHue(color_buffer_, hue_colors);
                
                if (!dtls_client_->sendFrame(hue_colors, ++sequence_number_)) {
                    spdlog::error("Failed to send frame. Reconnecting...");
                    dtls_client_->disconnect();
                    break; 
                }
                new_frame_ready_ = false;
            }

            auto end_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            
            if (elapsed < frame_duration) {
                std::this_thread::sleep_for(frame_duration - elapsed);
            }
        }
    }
}

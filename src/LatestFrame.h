#pragma once
#include <memory>
#include <vector>
#include <mutex>
#include "OpenRGB/Device.h"

template<typename T>
class LatestSlot {
public:
    LatestSlot() = default;

    void store(std::shared_ptr<T> v) {
        std::lock_guard<std::mutex> lock(mtx_);
        data_ = v;
    }

    std::shared_ptr<T> load() {
        std::lock_guard<std::mutex> lock(mtx_);
        return data_;
    }

    std::shared_ptr<T> exchange(std::shared_ptr<T> v) {
        std::lock_guard<std::mutex> lock(mtx_);
        std::shared_ptr<T> old = data_;
        data_ = v;
        return old;
    }

private:
    std::mutex mtx_;
    std::shared_ptr<T> data_{nullptr};
};

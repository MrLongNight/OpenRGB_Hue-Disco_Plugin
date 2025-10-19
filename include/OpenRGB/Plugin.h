#pragma once
#include "Device.h"

class Plugin {
public:
    virtual ~Plugin() = default;
    virtual void init() = 0;
    virtual void cleanup() = 0;
};

extern "C" Plugin* create_plugin();

void register_device(Device* device);

#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>

class Device;

// Mock implementation of the p_val union used by the OpenRGB plugin API
union p_val {
    int i_val;
    float f_val;
    const char* s_val;
    bool b_val;

    p_val() : i_val(0) {}
    p_val(int i) : i_val(i) {}
    p_val(float f) : f_val(f) {}
    p_val(const char* s) : s_val(s) {}
    p_val(bool b) : b_val(b) {}
    p_val(const std::string& s) : s_val(s.c_str()) {}
};

// Mock implementation of the plugin_api_t struct
struct plugin_api_t {
    void (*register_device)(Device*);
    void (*unregister_device)(Device*);
    int (*add_widget)(int, const char*, const std::map<std::string, p_val>&);
    void (*set_widget_property)(int, const char*, p_val);
    p_val (*get_widget_value)(int, const char*);
    void (*register_widget_callback)(int, std::function<void(const std::map<std::string, p_val>&)>);
};

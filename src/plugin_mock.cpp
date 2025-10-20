#include "OpenRGB/PluginAPI.h"
#include "OpenRGB/Device.h"
#include <iostream>

void mock_register_device(Device* device) {
    std::cout << "Mock: Registering device: " << device->device_name_ << std::endl;
}

void mock_unregister_device(Device* device) {
    std::cout << "Mock: Unregistering device: " << device->device_name_ << std::endl;
}

int mock_add_widget(int parent, const char* type, const std::map<std::string, p_val>& params) {
    std::cout << "Mock: Adding widget of type " << type << std::endl;
    return 1; // Dummy widget ID
}

void mock_set_widget_property(int widget_id, const char* prop, p_val value) {
    std::cout << "Mock: Setting widget " << widget_id << " property " << prop << std::endl;
}

p_val mock_get_widget_value(int widget_id, const char* prop) {
    std::cout << "Mock: Getting widget " << widget_id << " property " << prop << std::endl;
    return p_val("mock_value");
}

void mock_register_widget_callback(int widget_id, std::function<void(const std::map<std::string, p_val>&)> cb) {
    std::cout << "Mock: Registering callback for widget " << widget_id << std::endl;
}

// Global mock API instance
plugin_api_t mock_api = {
    mock_register_device,
    mock_unregister_device,
    mock_add_widget,
    mock_set_widget_property,
    mock_get_widget_value,
    mock_register_widget_callback
};

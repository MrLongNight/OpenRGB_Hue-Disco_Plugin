#include "HueClient.h"
#include "Logger.h"
#include <thread>
#include <chrono>

HueClient::HueClient(const std::string& bridge_ip, const std::string& username) : bridge_ip_(bridge_ip), http_client_(bridge_ip_.c_str()) {
    http_client_.set_default_headers({
        {"hue-application-key", username}
    });
}

bool HueClient::discoverBridge(std::string& ip) {
    // Placeholder for mDNS or other discovery logic
    spdlog::warn("Bridge discovery not implemented. Using configured IP.");
    return false;
}

bool HueClient::registerUserWithPushlink(std::string& username, std::string& clientkey) {
    json body = {
        {"devicetype", "openrgb_hue_entertainment#plugin"},
        {"generateclientkey", true}
    };
    
    for (int i = 0; i < 30; ++i) {
        auto res = http_client_.Post("/api", body.dump(), "application/json");
        if (res && res->status == 200) {
            try {
                json response = json::parse(res->body);
                if (response[0].contains("success")) {
                    username = response[0]["success"]["username"];
                    clientkey = response[0]["success"]["clientkey"];
                    spdlog::info("Successfully registered with Hue Bridge. Username: {}", username);
                    return true;
                }
            } catch (const std::exception& e) {
                spdlog::error("Failed to parse pushlink response: {}", e.what());
                return false;
            }
        }
        spdlog::info("Press the button on the Hue Bridge...");
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    spdlog::error("Pushlink registration timed out.");
    return false;
}

std::vector<HueEntertainmentArea> HueClient::getEntertainmentAreas() {
    std::vector<HueEntertainmentArea> areas;
    auto res = http_client_.Get("/clip/v2/resource/entertainment_configuration");
    if (res && res->status == 200) {
        try {
            json response = json::parse(res->body);
            for (const auto& area_data : response["data"]) {
                HueEntertainmentArea area;
                area.id = area_data["id"];
                area.name = area_data["metadata"]["name"];
                
                for(const auto& service : area_data["channels"]) {
                    // In v2, the locations are a bit more complex. 
                    // This is a simplified representation.
                    // You might need to make another request to resolve the lamp details.
                    area.lamp_uuids.push_back(service["service"]["rid"]);
                }
                areas.push_back(area);
            }
        } catch (const std::exception& e) {
            spdlog::error("Failed to parse entertainment areas: {}", e.what());
        }
    } else {
        spdlog::error("Failed to get entertainment areas: HTTP {}", res ? res->status : -1);
    }
    return areas;
}

bool HueClient::activateEntertainmentArea(const std::string& areaId) {
    json body = {{"action", "start"}};
    auto res = http_client_.Put(("/clip/v2/resource/entertainment_configuration/" + areaId).c_str(), body.dump(), "application/json");
    if (res && res->status == 200) {
        spdlog::info("Entertainment area {} activated.", areaId);
        return true;
    }
    spdlog::error("Failed to activate entertainment area {}: HTTP {}", areaId, res ? res->status : -1);
    return false;
}

bool HueClient::deactivateEntertainmentArea(const std::string& areaId) {
    json body = {{"action", "stop"}};
    auto res = http_client_.Put(("/clip/v2/resource/entertainment_configuration/" + areaId).c_str(), body.dump(), "application/json");
    if (res && res->status == 200) {
        spdlog::info("Entertainment area {} deactivated.", areaId);
        return true;
    }
    spdlog::error("Failed to deactivate entertainment area {}: HTTP {}", areaId, res ? res->status : -1);
    return false;
}

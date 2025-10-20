#include "HueClient.h"
#include "Logger.h"

HueClient::HueClient(const std::string& bridge_ip) {
    if (!bridge_ip.empty()) {
        http_client_ = std::make_unique<httplib::Client>(bridge_ip.c_str());
        http_client_->set_connection_timeout(5);
        http_client_->set_read_timeout(5);
    }
}

std::vector<HueEntertainmentArea> HueClient::getEntertainmentAreas(const std::string& username) {
    if (!http_client_) return {};
    std::vector<HueEntertainmentArea> areas;
    httplib::Headers headers = { {"hue-application-key", username} };

    auto res = http_client_->Get("/clip/v2/resource/entertainment_configuration", headers);
    if (res && res->status == 200) {
        try {
            json response = json::parse(res->body);
            for (const auto& area_data : response["data"]) {
                HueEntertainmentArea area;
                area.id = area_data["id"];
                area.name = area_data["metadata"]["name"];
                for(const auto& service : area_data["channels"]) {
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

bool HueClient::activateEntertainmentArea(const std::string& username, const std::string& areaId) {
    if (!http_client_) return false;
    json body = {{"action", "start"}};
    httplib::Headers headers = { {"hue-application-key", username} };
    auto res = http_client_->Put(("/clip/v2/resource/entertainment_configuration/" + areaId).c_str(), headers, body.dump(), "application/json");
    if (res && res->status == 200) {
        spdlog::info("Entertainment area {} activated.", areaId);
        return true;
    }
    spdlog::error("Failed to activate entertainment area {}: HTTP {}", areaId, res ? res->status : -1);
    return false;
}

bool HueClient::deactivateEntertainmentArea(const std::string& username, const std::string& areaId) {
    if (!http_client_) return false;
    json body = {{"action", "stop"}};
    httplib::Headers headers = { {"hue-application-key", username} };
    auto res = http_client_->Put(("/clip/v2/resource/entertainment_configuration/" + areaId).c_str(), headers, body.dump(), "application/json");
    if (res && res->status == 200) {
        spdlog::info("Entertainment area {} deactivated.", areaId);
        return true;
    }
    spdlog::error("Failed to deactivate entertainment area {}: HTTP {}", areaId, res ? res->status : -1);
    return false;
}

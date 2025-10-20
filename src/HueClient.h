#pragma once

#include <string>
#include <vector>
#include <memory>
#include <nlohmann/json.hpp>

#if !defined(CPPHTTPLIB_OPENSSL_SUPPORT)
#define CPPHTTPLIB_OPENSSL_SUPPORT
#endif
#include <httplib.h>

using json = nlohmann::json;

struct HueEntertainmentArea {
    std::string id;
    std::string name;
    std::vector<std::string> lamp_uuids;
};

class HueClient {
public:
    HueClient(const std::string& bridge_ip);

    std::vector<HueEntertainmentArea> getEntertainmentAreas(const std::string& username);
    bool activateEntertainmentArea(const std::string& username, const std::string& areaId);
    bool deactivateEntertainmentArea(const std::string& username, const std::string& areaId);

private:
    std::unique_ptr<httplib::Client> http_client_;
};

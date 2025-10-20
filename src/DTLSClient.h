#pragma once

#include <string>
#include <vector>
#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

struct DTLSHueColor {
    uint8_t r, g, b;
};

class DTLSClient {
public:
    DTLSClient(const std::string& bridge_ip, const std::string& username, const std::string& clientkey, const std::string& area_id);
    ~DTLSClient();

    bool connect();
    void disconnect();
    bool sendFrame(const std::vector<DTLSHueColor>& lampColors, const std::vector<std::string>& lampIds, uint32_t sequenceNumber);
    bool isConnected() const;

private:
    bool connected_ = false;
    std::string bridge_ip_;
    std::string username_;
    std::string area_id_;
    std::vector<unsigned char> psk_;

    mbedtls_net_context server_fd_;
    mbedtls_entropy_context entropy_;
    mbedtls_ctr_drbg_context ctr_drbg_;
    mbedtls_ssl_context ssl_;
    mbedtls_ssl_config conf_;
};

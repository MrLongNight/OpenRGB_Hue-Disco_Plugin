#include "DTLSClient.h"
#include "Logger.h"
#include <vector>
#include <nlohmann/json.hpp>
#include <cstring>
#include <algorithm>

// Helper to convert hex string to bytes
std::vector<unsigned char> hex_to_bytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

// Helper to convert UUID string to bytes
std::vector<unsigned char> uuid_to_bytes(std::string uuid) {
    uuid.erase(std::remove(uuid.begin(), uuid.end(), '-'), uuid.end());
    return hex_to_bytes(uuid);
}


DTLSClient::DTLSClient(const std::string& bridge_ip, const std::string& username, const std::string& clientkey, const std::string& area_id, const std::vector<Mapping>& mappings)
    : bridge_ip_(bridge_ip), username_(username), area_id_(area_id), mappings_(mappings) {
    
    mbedtls_net_init(&server_fd_);
    mbedtls_ssl_init(&ssl_);
    mbedtls_ssl_config_init(&conf_);
    mbedtls_ctr_drbg_init(&ctr_drbg_);
    mbedtls_entropy_init(&entropy_);

    psk_ = hex_to_bytes(clientkey);

    if (mbedtls_ctr_drbg_seed(&ctr_drbg_, mbedtls_entropy_func, &entropy_, NULL, 0) != 0) {
        spdlog::error("mbedtls_ctr_drbg_seed failed");
    }
}

DTLSClient::~DTLSClient() {
    disconnect();
    mbedtls_net_free(&server_fd_);
    mbedtls_ssl_free(&ssl_);
    mbedtls_ssl_config_free(&conf_);
    mbedtls_ctr_drbg_free(&ctr_drbg_);
    mbedtls_entropy_free(&entropy_);
}

bool DTLSClient::connect() {
    int ret;
    const char* pers = "dtls_client";

    if ((ret = mbedtls_net_connect(&server_fd_, bridge_ip_.c_str(), "2100", MBEDTLS_NET_PROTO_UDP)) != 0) {
        spdlog::error("mbedtls_net_connect returned {}", ret);
        return false;
    }

    if ((ret = mbedtls_ssl_config_defaults(&conf_, MBEDTLS_SSL_IS_CLIENT, MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        spdlog::error("mbedtls_ssl_config_defaults returned {}", ret);
        return false;
    }
    
    mbedtls_ssl_conf_rng(&conf_, mbedtls_ctr_drbg_random, &ctr_drbg_);
    
    // Set PSK
    if ((ret = mbedtls_ssl_conf_psk(&conf_, psk_.data(), psk_.size(), (const unsigned char*)username_.c_str(), username_.size())) != 0) {
        spdlog::error("mbedtls_ssl_conf_psk returned {}", ret);
        return false;
    }
    
    // Set ciphersuite
    static const int psk_cs[] = { MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256, 0 };
    mbedtls_ssl_conf_ciphersuites(&conf_, psk_cs);

    if ((ret = mbedtls_ssl_setup(&ssl_, &conf_)) != 0) {
        spdlog::error("mbedtls_ssl_setup returned {}", ret);
        return false;
    }

    mbedtls_ssl_set_bio(&ssl_, &server_fd_, mbedtls_net_send, mbedtls_net_recv, NULL);

    spdlog::info("Performing DTLS handshake...");
    while ((ret = mbedtls_ssl_handshake(&ssl_)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            spdlog::error("mbedtls_ssl_handshake returned -0x{:x}", -ret);
            return false;
        }
    }

    spdlog::info("DTLS handshake successful.");
    connected_ = true;
    return true;
}

void DTLSClient::disconnect() {
    mbedtls_ssl_close_notify(&ssl_);
    connected_ = false;
}

bool DTLSClient::isConnected() const {
    return connected_;
}

bool DTLSClient::sendFrame(const std::vector<DTLSHueColor>& lampColors, uint32_t sequenceNumber) {
    std::vector<unsigned char> payload;
    payload.reserve(16 + 16 + (lampColors.size() * 7));

    // Header
    const char* protocolName = "HueStream";
    payload.insert(payload.end(), protocolName, protocolName + 9);
    payload.push_back(0x02); // Major version
    payload.push_back(0x00); // Minor version
    payload.push_back((sequenceNumber >> 24) & 0xFF);
    payload.push_back((sequenceNumber >> 16) & 0xFF);
    payload.push_back((sequenceNumber >> 8) & 0xFF);
    payload.push_back(sequenceNumber & 0xFF);
    payload.push_back(0x00); // Reserved
    payload.push_back(0x00); // Reserved
    payload.push_back(0x00); // Color space (RGB)
    payload.push_back(0x00); // Reserved

    // Entertainment Area ID
    auto area_id_bytes = uuid_to_bytes(area_id_);
    payload.insert(payload.end(), area_id_bytes.begin(), area_id_bytes.end());

    // Light Channels
    for (size_t i = 0; i < lampColors.size(); ++i) {
        payload.push_back(i); // Channel ID
        // Convert to 16-bit big endian
        payload.push_back(lampColors[i].r);
        payload.push_back(lampColors[i].r);
        payload.push_back(lampColors[i].g);
        payload.push_back(lampColors[i].g);
        payload.push_back(lampColors[i].b);
        payload.push_back(lampColors[i].b);
    }

    int ret = mbedtls_ssl_write(&ssl_, payload.data(), payload.size());
    if (ret < 0) {
        spdlog::error("mbedtls_ssl_write returned {}", ret);
        return false;
    }
    return true;
}

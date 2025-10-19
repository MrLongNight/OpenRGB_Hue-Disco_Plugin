#include <catch2/catch_all.hpp>
#include "DTLSClient.h"
#include "Logger.h"
#include <mbedtls/ssl.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/timing.h>
#include <mbedtls/ssl_cookie.h>
#include <thread>
#include <nlohmann/json.hpp>

// Helper to convert hex string to bytes
std::vector<unsigned char> hex_to_bytes_test(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = (unsigned char)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

void mock_server(const std::string& psk_identity, const std::string& psk_key_hex, bool& success) {
    mbedtls_net_context listen_fd;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_timing_delay_context timer;
    mbedtls_ssl_cookie_ctx cookie_ctx;
    std::vector<unsigned char> psk_key_bytes;
    unsigned char client_ip[16] = {0};
    size_t cliip_len;
    unsigned char buf[1024];
    int ret = 0;

    mbedtls_net_init(&listen_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_ssl_cookie_init(&cookie_ctx);

    // Seeding the random number generator
    if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char*)"dtls_server", 11)) != 0) {
        goto cleanup;
    }

    // Setting up the listening socket
    if ((ret = mbedtls_net_bind(&listen_fd, NULL, "2100", MBEDTLS_NET_PROTO_UDP)) != 0) {
        goto cleanup;
    }

    // Setting up the SSL configuration
    if ((ret = mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_DATAGRAM, MBEDTLS_SSL_PRESET_DEFAULT)) != 0) {
        goto cleanup;
    }
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

    // Setup cookie callbacks
    if( ( ret = mbedtls_ssl_cookie_setup( &cookie_ctx, mbedtls_ctr_drbg_random, &ctr_drbg ) ) != 0 ) {
        goto cleanup;
    }
    mbedtls_ssl_conf_dtls_cookies( &conf, mbedtls_ssl_cookie_write, mbedtls_ssl_cookie_check, &cookie_ctx );

    psk_key_bytes = hex_to_bytes_test(psk_key_hex);
    if ((ret = mbedtls_ssl_conf_psk(&conf, psk_key_bytes.data(), psk_key_bytes.size(), (const unsigned char*)psk_identity.c_str(), psk_identity.size())) != 0) {
        goto cleanup;
    }

    static const int psk_cs[] = { MBEDTLS_TLS_PSK_WITH_AES_128_GCM_SHA256, 0 };
    mbedtls_ssl_conf_ciphersuites(&conf, psk_cs);

    if ((ret = mbedtls_ssl_setup(&ssl, &conf)) != 0) {
        goto cleanup;
    }

    mbedtls_ssl_set_timer_cb(&ssl, &timer, mbedtls_timing_set_delay, mbedtls_timing_get_delay);
    mbedtls_ssl_set_bio(&ssl, &listen_fd, mbedtls_net_send, mbedtls_net_recv, NULL);
    
    // Wait for a client connection
    ret = mbedtls_net_recv(&listen_fd, buf, sizeof(buf));
    if (ret < 0) {
        goto cleanup;
    }
    mbedtls_ssl_set_client_transport_id(&ssl, client_ip, cliip_len);
    
    // Handshake
    do {
        ret = mbedtls_ssl_handshake(&ssl);
    } while (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE);

    if (ret != 0) {
        goto cleanup;
    }

    // Read data
    ret = mbedtls_ssl_read(&ssl, buf, sizeof(buf) - 1);
    if (ret > 0) {
        if (memcmp(buf, "HueStream", 9) == 0) {
            success = true;
        }
    }

cleanup:
    mbedtls_ssl_cookie_free(&cookie_ctx);
    mbedtls_net_free(&listen_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
}

TEST_CASE("DTLSClient Handshake and Send", "[DTLSClient]") {
    std::string username = "testuser";
    std::string clientkey = "deadbeefdeadbeefdeadbeefdeadbeef";
    std::string area_id = "6eaf3b98-418d-48f3-89e4-a374cf9ef290";
    std::vector<Mapping> mappings = {{{0}, "lamp1"}};
    bool server_success = false;

    std::thread server_thread(mock_server, username, clientkey, std::ref(server_success));

    // Give server time to start
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    DTLSClient client("127.0.0.1", username, clientkey, area_id, mappings);
    REQUIRE(client.connect());

    std::vector<HueColor> colors = {{255, 0, 0}};
    REQUIRE(client.sendFrame(colors, 1));

    client.disconnect();

    server_thread.join();
    REQUIRE(server_success);
}

cmake_minimum_required(VERSION 3.16)

# Add test executable for mapping
add_executable(test_mapping test_mapping.cpp)
target_link_libraries(test_mapping PRIVATE Catch2::Catch2WithMain OpenRGB_HueEntertainment)

# Add test executable for DTLS mock server - DISABLED as per user request
# add_executable(test_dtls_mock_server test_dtls_mock_server.cpp)
# target_link_libraries(test_dtls_mock_server PRIVATE Catch2::Catch2WithMain OpenRGB_HueEntertainment mbedtls)

# Add tests to CTest
add_test(NAME MappingTests COMMAND test_mapping)
# add_test(NAME DtlsMockServerTest COMMAND test_dtls_mock_server)
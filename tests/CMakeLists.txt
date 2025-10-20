cmake_minimum_required(VERSION 3.16)

add_executable(test_mapping test_mapping.cpp)
target_link_libraries(test_mapping PRIVATE Catch2::Catch2WithMain OpenRGB_HueEntertainment)

add_test(NAME MappingTests COMMAND test_mapping)

cmake_minimum_required(VERSION 3.12)
project(libyara-wasm-test)
include(CMake/FetchCatch2.cmake)

set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

find_package(Yara REQUIRED MODULE)
find_package(OpenSSL REQUIRED MODULE)

add_executable(libyara-wasm-test ../YaraCC.cpp ../util.hpp ../util.cpp ../YaraCC.h YaraCC.test.cpp)
target_include_directories(libyara-wasm-test PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../)
target_link_libraries(libyara-wasm-test Yara::Yara OpenSSL::Crypto Catch2::Catch2WithMain)

catch_discover_tests(libyara-wasm-test)

cmake_minimum_required(VERSION 3.12)
project(libyara-wasm)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

ExternalProject_Add(yara
        URL https://github.com/VirusTotal/yara/archive/v3.8.1.tar.gz
        URL_HASH SHA512=32acb7b7336eebe2fe2c10ba6e8b23fea4e91148c289d15b53119bbed3dfe27508fbd90aef5c036a5b6a7e31e0f73ec8885e30093c598f9bc04bd9d64ac0ac7e
        CONFIGURE_COMMAND ./bootstrap.sh
        COMMAND emconfigure ./configure
        CMAKE_GENERATOR emmake make
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS
        <SOURCE_DIR>/libyara/.libs/libyara.a
        <SOURCE_DIR>/libyara/.libs/libyara.so
        <SOURCE_DIR>/libyara/.libs/libyara.so.3
        <SOURCE_DIR>/libyara/.libs/libyara.so.3.8.1
        INSTALL_COMMAND ""
        )

add_library(libyara STATIC IMPORTED GLOBAL)
set_target_properties(libyara
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/.libs/libyara.a
        )
add_dependencies(libyara yara)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/include
        ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/.libs
        ${CMAKE_CURRENT_SOURCE_DIR}
        $ENV{EMSCRIPTEN}/system/include/
)

add_executable(libyara-wasm main.cpp util.hpp util.cpp)
add_dependencies(libyara-wasm libyara)
target_link_libraries(libyara-wasm libyara)
set_target_properties(libyara-wasm
        PROPERTIES SUFFIX ".js"
        LINK_FLAGS "-O3 --bind"
)

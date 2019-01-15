cmake_minimum_required(VERSION 3.12)
project(libyara-wasm)

set(CMAKE_CXX_STANDARD 14)

include(ExternalProject)

include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/include
        ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/.libs
        ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix/src/openssl/include/openssl
        ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix/src/openssl
        ${CMAKE_CURRENT_SOURCE_DIR}
        $ENV{EMSCRIPTEN}/system/include/
)

ExternalProject_Add(openssl
        URL https://www.openssl.org/source/openssl-1.0.2q.tar.gz
        URL_HASH SHA256=5744cfcbcec2b1b48629f7354203bc1e5e9b5466998bbccc5b5fcde3b18eb684
        CONFIGURE_COMMAND emmake bash -c "./Configure -no-asm -no-apps no-ssl2 no-ssl3 no-hw no-deprecated shared --openssldir=built linux-generic32 --prefix=$ENV{EMSCRIPTEN}/system"
        COMMAND sed -i "s|^CROSS_COMPILE.*$|CROSS_COMPILE=|g" Makefile
        COMMAND sed -i "s|^build_all.*$|build_all: build_libs build_apps|g" Makefile
        #COMMAND sed -i "s|^build_libs.*$|build_libs: build_libcrypto|g" Makefile
        #COMMAND sed -i "s|^LIBS=.*$|LIBS=   libcrypto.a|g" Makefile
        #COMMAND sed -i "s|^SHARED_LIBS=.*$|SHARED_LIBS=$(SHARED_CRYPTO)|g" Makefile
        BUILD_COMMAND $(MAKE) -s NO_FILESYSTEM=1
        BUILD_IN_SOURCE 1
        BUILD_BYPRODUCTS
        <SOURCE_DIR>/libcrypto.a
        <SOURCE_DIR>/libcrypto.so
        <SOURCE_DIR>/libcrypto.so.1.0.2
        #INSTALL_COMMAND cp -t $ENV{EMSCRIPTEN}/lib/ libcrypto.a libcrypto.so libcrypto.so.1.0.0
        INSTALL_COMMAND emmake make install
        )
add_library(libcrypto STATIC IMPORTED GLOBAL)
set_target_properties(libcrypto
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix/src/openssl/libcrypto.a
        )
add_dependencies(libcrypto openssl)

ExternalProject_Add(yara
        URL https://github.com/VirusTotal/yara/archive/v3.8.1.tar.gz
        URL_HASH SHA512=32acb7b7336eebe2fe2c10ba6e8b23fea4e91148c289d15b53119bbed3dfe27508fbd90aef5c036a5b6a7e31e0f73ec8885e30093c598f9bc04bd9d64ac0ac7e
        CONFIGURE_COMMAND emconfigure ./bootstrap.sh
        COMMAND sed -i "s|OPENSSL_VERSION_NUMBER.*$|OPENSSL_VERSION_NUMBER < 0x10002000L|" libyara/libyara.c
        COMMAND emconfigure ./configure
        BUILD_COMMAND $(MAKE) -s NO_FILESYSTEM=1
        DEPENDS libcrypto
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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../dist)
add_executable(libyara-wasm main.cpp util.hpp util.cpp YaraCC.hpp)
add_dependencies(libyara-wasm libyara libcrypto)
target_link_libraries(libyara-wasm libyara libcrypto)
set_target_properties(libyara-wasm
        PROPERTIES SUFFIX ".js"
        LINK_FLAGS "--bind -O3 -s MODULARIZE=1 -s NO_FILESYSTEM=1 -s SINGLE_FILE=1 -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0"
)

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
        URL https://www.openssl.org/source/openssl-1.1.1d.tar.gz 
	URL_HASH SHA256=1e3a91bc1f9dfce01af26026f856e064eab4c8ee0a8f457b5ae30b40b8b711f2
	CONFIGURE_COMMAND emmake bash -c "./Configure -no-asm linux-generic32 --prefix=$ENV{EMSCRIPTEN}/system CFLAGS='-D__STDC_NO_ATOMICS__=1'"
        COMMAND sed -i "s|^CROSS_COMPILE.*$|CROSS_COMPILE=|g" Makefile
        BUILD_COMMAND $(MAKE) build_generated libcrypto.a libssl.a -s NO_FILESYSTEM=1
        BUILD_IN_SOURCE 1
	INSTALL_COMMAND bash -c "cp libssl.a libcrypto.a $ENV{EMSCRIPTEN}/system/lib; cp -R include/openssl $ENV{EMSCRIPTEN}/system/include"
	
	)
add_library(libcrypto STATIC IMPORTED GLOBAL)
set_target_properties(libcrypto
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix/src/openssl/libcrypto.a
        )
add_dependencies(libcrypto openssl)

ExternalProject_Add(yara
        URL https://github.com/VirusTotal/yara/archive/v3.11.0.tar.gz
	URL_HASH SHA512=26d800284a2de07195e4a480eb7125b1f5ad0265b284bfbeee10dbab162ea0f419c9e2ea5c4ed5fa25a46a2c9e3c6b077dfe1cafece79251e20d5d70f91b80a8
        CONFIGURE_COMMAND emconfigure ./bootstrap.sh
        COMMAND emconfigure ./configure
        BUILD_COMMAND $(MAKE) -s NO_FILESYSTEM=1
        DEPENDS libcrypto
        BUILD_IN_SOURCE 1
	INSTALL_COMMAND cp libyara/.libs/libyara.a $ENV{EMSCRIPTEN}/system/lib
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
        LINK_FLAGS "--bind -O3 -s MODULARIZE=1 -s NO_FILESYSTEM=1 -s SINGLE_FILE=1 -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s ENVIRONMENT='node, worker'"
)

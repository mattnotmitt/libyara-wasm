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
        ${EMSCRIPTEN_SYSROOT}/include/
)
# 0x1010111fL
ExternalProject_Add(openssl
        URL https://www.openssl.org/source/openssl-1.1.1q.tar.gz
        URL_HASH SHA256=d7939ce614029cdff0b6c20f0e2e5703158a489a72b2507b8bd51bf8c8fd10ca
        CONFIGURE_COMMAND emconfigure ./Configure shared no-afalgeng no-aria no-asan no-asm no-async no-autoalginit no-autoerrinit no-autoload-config no-bf no-blake2 no-camellia no-capieng no-cast no-chacha no-cmac no-cms no-comp no-crypto-mdebug no-crypto-mdebug-backtrace no-ct  no-des no-devcryptoeng no-dgram no-dh no-dsa no-dtls no-dtls1 no-dtls1-method no-dynamic-engine no-ec no-ec2m no-ecdh no-ecdsa no-egd no-engine no-err no-external-tests no-fuzz-afl no-fuzz-libfuzzer no-gost no-heartbeats no-hw no-hw-padlock no-idea no-md2 no-md4 no-mdc2 no-msan no-multiblock no-nextprotoneg no-ocb no-ocsp no-poly1305 no-posix-io no-psk no-rc2 no-rc4 no-rc5 no-rfc3779 no-rmd160 no-scrypt no-sctp no-seed no-sm2 no-sm3 no-sm4 no-sock no-srp no-srtp no-ssl3 no-ssl3-method no-ssl-trace no-stdio no-tests no-threads no-tls no-tls1 no-tls1-method no-ts no-ubsan no-ui-console no-unit-test no-weak-ssl-ciphers no-whirlpool 
                                        shared linux-generic32
                                        --prefix=${EMSCRIPTEN_SYSROOT} --openssldir=${EMSCRIPTEN_SYSROOT}
                                        --cross-compile-prefix=""
        #COMMAND sed -i "s|^CROSS_COMPILE.*$|CROSS_COMPILE=|g" Makefile
        BUILD_COMMAND $(MAKE) build_generated build_libs_nodep libssl.a libcrypto.a -s NO_FILESYSTEM=1
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND make install_dev
        )
add_library(libcrypto STATIC IMPORTED GLOBAL)
set_target_properties(libcrypto
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/openssl-prefix/src/openssl/libcrypto.a
        )
add_dependencies(libcrypto openssl)

ExternalProject_Add(yara
        URL https://github.com/VirusTotal/yara/archive/v4.2.3.tar.gz
        URL_HASH SHA512=1df23837d14d3f9f72db901190e7412241514a8914d72b753df32ad8a44c7a49ad8f14941a95de83754e4d1dd0f23d712d8e6a43916343341b92cb1bc66f3c09
        CONFIGURE_COMMAND emconfigure ./bootstrap.sh
        COMMAND emconfigure ./configure --with-crypto "LDFLAGS=-L${EMSCRIPTEN_SYSROOT}/lib -sERROR_ON_UNDEFINED_SYMBOLS=0 -sEMULATE_FUNCTION_POINTER_CASTS=1"
        BUILD_COMMAND $(MAKE) -s NO_FILESYSTEM=1
        DEPENDS libcrypto
        BUILD_IN_SOURCE 1
        INSTALL_COMMAND cp libyara/.libs/libyara.a ${EMSCRIPTEN_SYSROOT}
        )

add_library(libyara STATIC IMPORTED GLOBAL)
set_target_properties(libyara
        PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/yara-prefix/src/yara/libyara/.libs/libyara.a
        )
add_dependencies(libyara yara)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../dist)
add_executable(libyara-wasm YaraCC.cpp util.hpp util.cpp YaraCC.h)
add_dependencies(libyara-wasm libyara libcrypto)
target_include_directories(libyara-wasm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(libyara-wasm libyara libcrypto)
set_target_properties(libyara-wasm
        PROPERTIES SUFFIX ".js"
        LINK_FLAGS "--bind -Oz -s WASM=1 --closure 1 -s MODULARIZE=1 -s NO_FILESYSTEM=1 -s ALLOW_MEMORY_GROWTH=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -s SINGLE_FILE=1 -s ENVIRONMENT=node,worker -sEXPORTED_RUNTIME_METHODS=run -sEMULATE_FUNCTION_POINTER_CASTS=1"
        )

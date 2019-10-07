# libyara-wasm
[![Build Status](https://github.com/mattnotmitt/libyara-wasm/workflows/Build%20%26%20Deploy/badge.svg)](https://github.com/mattnotmitt/libyara-wasm/actions?workflow=Build%20%26%20Deploy)

WASM-based JS Library using a c++ wrapper around [libyara](https://github.com/virustotal/yara). Currently only exposes methods required by [CyberChef](https://github.com/gchq/CyberChef) but may potentially expose more in the future. No documentation at the moment.
## Build

```sh
mkdir build && cd build
emconfigure cmake ..
emmake make -jx # where x is number of threads to build with
```

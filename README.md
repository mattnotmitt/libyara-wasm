# libyara-wasm
Basic compilation of libyara to wasm and to run some tests in a html file. Further plans include basic yara functionality tests before potential integration with CyberChef. 
(I don't exactly understand the wasm/js integration layer just yet, so we'll see how it goes)
Some modules unavailable at the moment, but this isn't necessarily always going to be true.

## Build

```sh
mkdir build && cd build
emconfigure cmake ..
emmake make -jx # where x is number of threads to build with
```

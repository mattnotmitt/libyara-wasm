# libyara-wasm

Honestly can't believe this compiles, I don't even know if this runs any actual yara code but i'm so tired at this point i don't even care.

## Build

```sh
mkdir build && cd build
emconfigure cmake ..
emmake make -jx # where x is number of threads to build with
```

# inferior_osyncstream

inferior_osyncstream is similler to C++20 `std::osyncstream`, implemented by C++17.

Alocator is not supported.

## build && test

C++17 compiler and cmake 3.7 or later is required.

```
git clone https://github.com/yumetodo/inferor_osyncstream.git
cd inferor_osyncstream
git submoule update --init --depth=1
cmake -B build .
cmake --build build --target ci
```

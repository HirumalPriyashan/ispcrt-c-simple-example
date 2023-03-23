# Simple example of ISPCRT

This is a simple example of using ISPC runtime with C which is a modified verison of simple xpu exmple of ISPC example set.

## About the implementation

This implementation try to use ispcrt without using `struct`.

## Build instructions

Compile ISPC kernel for CPU:

```sh
ispc -I /home/ispc_package/include/ispcrt --arch=x86-64 --target=sse4-i32x4,avx1-i32x8,avx2-i32x8,avx512knl-x16,avx512skx-x16 --woff --pic --opt=disable-assertions -h simple_ispc.h -o simple.dev.o simple.ispc
```

Produce a library from object files:
```sh
/usr/bin/c++ -fPIC -shared -Wl,-soname,libxe_simple.so -o libxe_simple.so
simple.dev*.o
```

Compile and link host code:
```
/usr/bin/c++ -DISPCRT -isystem /home/ispc_package/include/ispcrt -fPIE -o host_simple simple.c -lispcrt -L/home/ispc_package/lib64 -Wl,-rpath,/home/ispc_package/lib64
```

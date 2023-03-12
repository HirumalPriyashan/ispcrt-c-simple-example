#!/bin/bash

export LD_LIBRARY_PATH=/usr/lib/llvm-14/lib:/home/hirumal_priyashan/looping-in-loopy/fyp/ispcrt-c-simple-example/float

ispc -I ../../ispc-trunk-linux/include/ispcrt -g -o vec-add.dev.o vec-add.ispc

/usr/bin/cc -fPIC -shared -o libsimple.so vec-add.dev.o
/usr/bin/cc simple.c -fPIE -g -o host_simple -lispcrt -Wl,-rpath,../../ispc-trunk-linux/lib64

# valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./host_simple
./host_simple

rm vec-add.dev.o host_simple libsimple.so
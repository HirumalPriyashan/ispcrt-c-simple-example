#!/bin/bash

export LD_LIBRARY_PATH=/usr/lib/llvm-14/lib:/home/hirumal_priyashan/looping-in-loopy/fyp/ispcrt-c-simple-example

ispc -I ../ispc-v1.18.1-linux/include/ispcrt -g -o simple.dev.o simple.ispc
ispc -I ../ispc-v1.18.1-linux/include/ispcrt -g -o vac-add.dev.o vec-add.ispc

/usr/bin/cc -fPIC -shared -o libvecadd.so vac-add.dev.o
/usr/bin/cc -fPIC -shared -o libsimple.so simple.dev.o
/usr/bin/cc simple.c -fPIE -g -o host_simple -lispcrt -Wl,-rpath,../ispc-v1.18.1-linux/lib64

valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt ./host_simple

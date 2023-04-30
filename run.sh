#!/bin/bash

mkdir -p build && cd build
cmake ..
make

./ispc-vec-add

rm -rf build
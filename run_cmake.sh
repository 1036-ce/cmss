#!/bin/bash

rm -rf build
mkdir build
cd build
cmake ..
make test_x86

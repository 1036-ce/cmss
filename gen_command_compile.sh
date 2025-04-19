#!/bin/bash

make clean
bear -- make -j `nproc`

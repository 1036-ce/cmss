#!/bin/bash

export AFLNET="$(pwd)"
export WORKDIR="$(dirname "$(pwd)")"
export PATH=$PATH:$AFLNET
export AFL_PATH=$AFLNET

LLVM_MODE_DIR="$AFLNET"/llvm_mode
LIVE555="$WORKDIR"/live555
LIVE555_TESTPROGS="$LIVE555"/testProgs


cd "$AFLNET"
make all
cd "$LLVM_MODE_DIR"
make

rm -rf "$LIVE555"
cd "$WORKDIR"
git clone https://github.com/rgaufman/live555.git "$LIVE555"

cd "$LIVE555"
git checkout ceeb4f4
patch -p1 < "$AFLNET"/tutorials/live555/ceeb4f4.patch
./genMakefiles linux
make clean all

cd "$AFLNET"
cp "$AFLNET"/tutorials/live555/sample_media_sources/*.* ./

#!/bin/bash

AFLNET="$(pwd)"
WORKDIR="$(dirname "$(pwd)")"
LLVM_MODE_DIR="$AFLNET"/llvm_mode
LIVE555="$WORKDIR"/live555
LIVE555_TESTPROGS="$LIVE555"/testProgs

cd "$AFLNET"
make all
cd "$LLVM_MODE_DIR"
make

cd "$WORKDIR"
git clone https://github.com/rgaufman/live555.git "$LIVE555"

cd "$LIVE555"
git checkout ceeb4f4
patch -p1 < "$AFLNET"/tutorials/live555/ceeb4f4.patch
./genMakefiles linux
make clean all

cd "$LIVE555_TESTPROGS"

cp "$AFLNET"/tutorials/live555/sample_media_sources/*.* ./

afl-fuzz -d -i "$AFLNET"/tutorials/live555/in-rtsp \
-o out-live555 -N tcp://127.0.0.1/8554 \
-x "$AFLNET"/tutorials/live555/rtsp.dict \
-P RTSP -D 10000 -q 3 -s 3 -E -K -R ./testOnDemandRTSPServer 8554

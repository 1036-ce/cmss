#!/bin/bash

export AFLNET="$(pwd)"
export WORKDIR="$(dirname "$(pwd)")"
export PATH=$PATH:$AFLNET
export AFL_PATH=$AFLNET
# export AFL_NO_UI=1
export AFL_SKIP_CPUFREQ=1
export AFL_NO_AFFINITY=1

# LLVM_MODE_DIR="$AFLNET"/llvm_mode
# LIVE555="$WORKDIR"/live555
# LIVE555_TESTPROGS="$LIVE555"/testProgs
TEST_DIR="${AFLNET}/test"
INPUT_DIR="input"
OUTPUT_DIR="output"
EXTRAS_DIR="rtsp.dict"

cd "${TEST_DIR}"

sudo sh -c "echo core >/proc/sys/kernel/core_pattern"
sudo sh -c "cd /sys/devices/system/cpu && echo performance | tee cpu*/cpufreq/scaling_governor"

# afl-fuzz -d -i "${INPUT_DIR}" \
# -o "${OUTPUT_DIR}" \
# -N tcp://127.0.0.1/8554 \
# -x "${EXTRAS_DIR}" \
# -P RTSP -D 10000 -q 3 -s 3 -E -K -R \
# ./testOnDemandRTSPServer 8554
afl-fuzz -i "${INPUT_DIR}" \
-N tcp://127.0.0.1/8554 \
-x "${EXTRAS_DIR}" \
-P RTSP -D 10000 \
./testOnDemandRTSPServer 8554

cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_COMPILER /usr/bin/gcc-13)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}) 


project(aflnet LANGUAGES C)

# extrace VERSION from config.h
file(READ "${CMAKE_SOURCE_DIR}/config.h" CONFIG_H_CONTENTS)
string(REGEX MATCH "#define VERSION [ \t]+ \"[0-9]\.[0-9]+[a-zA-Z]\"" VERSION_LINE "${CONFIG_H_CONTENTS}")
string(REGEX MATCH "[0-9]\.[0-9]+[a-zA-Z]" VERSION "${VERSION_LINE}")

set(PREFIX /usr/local)
set(BIN_PATH ${PREFIX}/bin)
set(HELPER_PATH ${PREFIX}/lib/afl)
set(DOC_PATH ${PREFIX}/share/doc/afl)
set(MISC_PATH ${PREFIX}/share/afl)

set(PROGS 
  afl-gcc
  afl-fuzz
  afl-replay
  aflnet-replay
  afl-showmap
  afl-tmin
  afl-gotcpu
  afl-analyze
)

set(SH_PROGS
  afl-plot
  afl-cmin
  afl-whatsup
)

set(CMAKE_C_FLAGS " -O3 -funroll-loops")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -D_FORTIFY_SOURCE=2 -g -Wno-pointer-sign -Wno-unused-result")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DAFL_PATH=\\\"${HELPER_PATH}\\\" -DDOC_PATH=\\\"${DOC_PATH}\\\" -DBIN_PATH=\\\"${BIN_PATH}\\\"")

# message(STATUS ${CMAKE_C_FLAGS})

set(COMM_HDR 
  alloc-inl.h
  config.h
  debug.h
  types.h
)

set(LINK_LIBS dl gvc cgraph m cap)


add_executable(afl-gcc afl-gcc.c ${COMM_HDR})

add_custom_command(
  TARGET afl-gcc POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink afl-gcc ${CMAKE_SOURCE_DIR}/afl-g++
  COMMAND ${CMAKE_COMMAND} -E create_symlink afl-gcc ${CMAKE_SOURCE_DIR}/afl-clang
  COMMAND ${CMAKE_COMMAND} -E create_symlink afl-gcc ${CMAKE_SOURCE_DIR}/afl-clang++
)
# set additional clean files: afl-g++ afl-clang afl-clang++
set_property(
  TARGET afl-gcc 
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES 
  ${CMAKE_SOURCE_DIR}/afl-g++ 
  ${CMAKE_SOURCE_DIR}/afl-clang 
  ${CMAKE_SOURCE_DIR}/afl-clang++
)

add_executable(afl-as afl-as.c afl-as.h ${COMM_HDR})
add_custom_command(
  TARGET afl-as POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E create_symlink afl-as ${CMAKE_SOURCE_DIR}/as
)
# set additional clean files: as
set_property(
  TARGET afl-gcc 
  APPEND
  PROPERTY ADDITIONAL_CLEAN_FILES ${CMAKE_SOURCE_DIR}/as
)

add_executable(afl-fuzz afl-fuzz.c ${COMM_HDR} aflnet.c aflnet.h)
target_link_libraries(afl-fuzz ${LINK_LIBS})

add_executable(afl-replay afl-replay.c ${COMM_HDR} aflnet.c aflnet.h)
target_link_libraries(afl-replay ${LINK_LIBS})

add_executable(aflnet-replay aflnet-replay.c ${COMM_HDR} aflnet.c aflnet.h)
target_link_libraries(aflnet-replay ${LINK_LIBS})

add_executable(afl-showmap afl-showmap.c ${COMM_HDR})

add_executable(afl-tmin afl-tmin.c ${COMM_HDR})

add_executable(afl-analyze afl-analyze.c ${COMM_HDR})

add_executable(afl-gotcpu afl-gotcpu.c ${COMM_HDR})

add_custom_target(
  llvm_mode
  COMMAND make
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/llvm_mode
  VERBATIM
)

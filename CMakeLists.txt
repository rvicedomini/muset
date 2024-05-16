cmake_minimum_required(VERSION 3.15)
project(UTGMAT)

set(CMAKE_CXX_STANDARD 17)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()

MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CUSTOM_INSTRUCTION_FLAGS "")
MESSAGE(STATUS "Compiling for processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (UNIX AND (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64"))
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2") # for vectorized instructions
  list(APPEND CUSTOM_INSTRUCTION_FLAGS "-msse4.2")

  if (NOT CONDA_BUILD) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    list(APPEND CUSTOM_INSTRUCTION_FLAGS "-march=native")
  endif()

  if(NOT NO_BMI2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2") # for hardware popcount and pdep
    list(APPEND CUSTOM_INSTRUCTION_FLAGS "-mbmi2")
  endif()
  MESSAGE(STATUS "Compiling with custom instruction flags: ${CUSTOM_INSTRUCTION_FLAGS}")
endif()

MESSAGE(STATUS "SSHash will use a maximum kmer length of 63")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSSHASH_USE_MAX_KMER_LENGTH_63")

if (UNIX)

  MESSAGE(STATUS "Compiling with flags: -std=c++17 -O3 -ggdb -pthread -Wall -Wall -Wextra -Wno-unused-function -pedantic")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wall -Wextra -Wno-unused-function -pedantic")

  # Flags for PTHash:
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") # for multi-threading

  if (SSHASH_USE_SANITIZERS)
    MESSAGE(STATUS "Using sanitizers. Compiling with flags: -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  endif()

endif()

find_package(ZLIB REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(Z_LIB_SOURCES
  include/gz/zip_stream.cpp
)

set(SSHASH_SOURCES
  include/dictionary.cpp
  include/info.cpp
  # include/dump.cpp
  # include/statistics.cpp
  include/builder/build.cpp
)

add_library(sshash_static STATIC ${Z_LIB_SOURCES} ${SSHASH_SOURCES})
# target_include_directories(sshash_static PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(UTGMAT_SOURCES
  src/km_basic_filter.cpp
  src/km_diff.cpp
  src/km_fasta.cpp
  src/km_tools.cpp
  src/km_merge.cpp
  src/km_reverse.cpp
  src/km_select.cpp
  src/km_unitig.cpp
)

add_executable(kmtools ${UTGMAT_SOURCES})
target_link_libraries(kmtools ZLIB::ZLIB sshash_static Threads::Threads)

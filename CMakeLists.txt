cmake_minimum_required(VERSION 3.15)
project(kmat_tools LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()

option(CONDA_BUILD "Build inside conda env." OFF)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif ()

MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CUSTOM_INSTRUCTION_FLAGS "")
MESSAGE(STATUS "Compiling for processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (UNIX AND (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64"))
  
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2") # for vectorized instructions
  list(APPEND CUSTOM_INSTRUCTION_FLAGS "-msse4.2")

  if(NOT NO_BMI2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2") # for hardware popcount and pdep
    list(APPEND CUSTOM_INSTRUCTION_FLAGS "-mbmi2")
  endif()

  if (NOT CONDA_BUILD) 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
    list(APPEND CUSTOM_INSTRUCTION_FLAGS "-march=native")
  endif()

  MESSAGE(STATUS "Compiling with custom instruction flags: ${CUSTOM_INSTRUCTION_FLAGS}")
endif()

MESSAGE(STATUS "SSHash will use a maximum kmer length of 63")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSSHASH_USE_MAX_KMER_LENGTH_63")

if (UNIX)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-ignored-qualifiers -Wno-char-subscripts -Wno-maybe-uninitialized -Wno-unused-function -pedantic")  

  if (SSHASH_USE_SANITIZERS)
    MESSAGE(STATUS "Using sanitizers. Compiling with flags: -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  endif()

endif()

find_package(ZLIB REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_library(headers INTERFACE)
target_include_directories(headers INTERFACE ${CMAKE_SOURCE_DIR}/external)
add_library(links INTERFACE)
target_link_libraries(links INTERFACE pthread)
add_library(deps INTERFACE)

add_subdirectory(external)

set(KMAT_TOOLS_SOURCES
  src/km_basic_filter.cpp
  src/km_diff.cpp
  src/km_fafmt.cpp
  src/km_fasta.cpp
  src/km_ktfilter.cpp
  src/km_merge.cpp
  src/km_reverse.cpp
  src/km_select.cpp
  src/km_tools.cpp
  src/km_unitig.cpp
  src/km_convert.cpp
)

add_executable(kmat_tools ${KMAT_TOOLS_SOURCES})
target_link_libraries(kmat_tools PRIVATE headers links deps)
target_link_options(kmat_tools PUBLIC -export-dynamic)

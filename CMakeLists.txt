cmake_minimum_required(VERSION 3.15)
project(kmat_tools LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CONDA_BUILD "Build inside conda env." OFF)

## Disallow in-tree builds
if(${PROJECT_SOURCE_DIR} STREQUAL ${PROJECT_BINARY_DIR})
    message(FATAL_ERROR "In-source build not allowed. Create a \"build\" directory and run CMake from there.")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()
MESSAGE(STATUS "CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-ignored-qualifiers -Wno-char-subscripts -Wno-maybe-uninitialized -Wno-unused-function -pedantic")

MESSAGE(STATUS "Compiling for processor: " ${CMAKE_HOST_SYSTEM_PROCESSOR})
if (UNIX AND (CMAKE_HOST_SYSTEM_PROCESSOR STREQUAL "x86_64"))
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse4.2") # for vectorized instructions
  if(NOT NO_BMI2)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2") # for hardware popcount and pdep
  endif()
endif()

if (NOT CONDA_BUILD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

find_package(ZLIB REQUIRED)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

set(includes "")
set(deps_libs "")
set(deps "")

# add_library(headers INTERFACE)
# target_include_directories(headers INTERFACE ${CMAKE_SOURCE_DIR}/external)
# add_library(links INTERFACE)
# target_link_libraries(links INTERFACE)
# add_library(deps INTERFACE)

include(${PROJECT_SOURCE_DIR}/cmake/external.cmake)

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

add_executable(kmat_tools ${KMAT_TOOLS_SOURCES} ${SSHASH_SOURCES})

target_include_directories(kmat_tools PRIVATE ${includes})
target_link_libraries(kmat_tools ${deps_libs})
add_dependencies(kmat_tools ${deps})

# target_link_libraries(kmat_tools headers links deps)

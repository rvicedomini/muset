include(ExternalProject)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(EXTERNAL_BINDIR ${CMAKE_BINARY_DIR})

MESSAGE(STATUS "EXTERNAL_DIR: " ${EXTERNAL_DIR})
MESSAGE(STATUS "EXTERNAL_BINDIR: " ${EXTERNAL_BINDIR})

# kmtricks-api

list(APPEND includes ${EXTERNAL_DIR}/kmtricks/include)

# fmt

ExternalProject_Add(FMT
  PREFIX FMT
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/fmt
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON
             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  INSTALL_COMMAND ""
  LOG_CONFIGURE ON
  LOG_BUILD ON
)

# target_include_directories(headers SYSTEM INTERFACE ${EXTERNAL_DIR}/fmt/include)
# target_link_directories(links INTERFACE ${EXTERNAL_BINDIR}/FMT/src/FMT-build)
# target_link_libraries(links INTERFACE fmt)
# add_dependencies(deps FMT)

list(APPEND includes ${EXTERNAL_DIR}/fmt/include)
list(APPEND deps_libs ${EXTERNAL_BINDIR}/FMT/src/FMT-build/libfmt.a)
list(APPEND deps FMT)

# LZ4

ExternalProject_Add(LZ4
    PREFIX LZ4
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/lz4
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF
               -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON
               -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
)

# target_include_directories(headers SYSTEM INTERFACE ${EXTERNAL_DIR}/lz4/lib)
# target_link_directories(links INTERFACE ${EXTERNAL_BINDIR}/LZ4/src/LZ4-build)
# target_link_libraries(links INTERFACE lz4)
# add_dependencies(deps LZ4)

list(APPEND includes ${EXTERNAL_DIR}/lz4/lib)
list(APPEND deps_libs ${EXTERNAL_BINDIR}/LZ4/src/LZ4-build/liblz4.a)
list(APPEND deps LZ4)

# SSHash

MESSAGE(STATUS "SSHash will use a maximum kmer length of 63")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSSHASH_USE_MAX_KMER_LENGTH_63")

set(SSHASH_SOURCES
    ${EXTERNAL_DIR}/sshash/dictionary.cpp
    ${EXTERNAL_DIR}/sshash/info.cpp
    ${EXTERNAL_DIR}/sshash/builder/build.cpp
    ${EXTERNAL_DIR}/sshash/gz/zip_stream.cpp
)

add_library(sshash_static STATIC ${SSHASH_SOURCES})
# target_link_libraries(links INTERFACE sshash_static)
list(APPEND deps_libs sshash_static)
list(APPEND deps sshash_static)

# zlib + threads

# target_link_libraries(links INTERFACE ZLIB::ZLIB)
# target_link_libraries(links INTERFACE Threads::Threads)
list(APPEND deps_libs ZLIB::ZLIB)
list(APPEND deps_libs Threads::Threads)

include(ExternalProject)

set(EXTERNAL_DIR ${CMAKE_SOURCE_DIR}/external)
set(EXTERNAL_BINDIR ${CMAKE_BINARY_DIR}/external)

# kmtricks-api

target_include_directories(headers SYSTEM INTERFACE ${EXTERNAL_DIR}/kmtricks/include)

# fmt

ExternalProject_Add(FMT
  PREFIX FMT
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/fmt
  INSTALL_COMMAND ""
  LOG_CONFIGURE ON
  LOG_BUILD ON
)
target_include_directories(headers SYSTEM INTERFACE ${EXTERNAL_DIR}/fmt/include)
target_link_directories(links INTERFACE ${EXTERNAL_BINDIR}/FMT/src/FMT-build)
target_link_libraries(links INTERFACE fmt)
add_dependencies(deps FMT)

# LZ4

ExternalProject_Add(LZ4
    PREFIX LZ4
    SOURCE_DIR ${PROJECT_SOURCE_DIR}/external/lz4
    SOURCE_SUBDIR build/cmake
    CMAKE_ARGS -DBUILD_STATIC_LIBS=ON -DBUILD_SHARED_LIBS=OFF -DCMAKE_BUILD_TYPE=Release
    INSTALL_COMMAND ""
    LOG_CONFIGURE ON
    LOG_BUILD ON
)
target_include_directories(headers SYSTEM INTERFACE ${EXTERNAL_DIR}/lz4/lib)
target_link_directories(links INTERFACE ${EXTERNAL_BINDIR}/LZ4/src/LZ4-build)
target_link_libraries(links INTERFACE lz4)
add_dependencies(deps LZ4)

# SSHash

set(SSHASH_SOURCES
    ${EXTERNAL_DIR}/sshash/dictionary.cpp
    ${EXTERNAL_DIR}/sshash/info.cpp
    ${EXTERNAL_DIR}/sshash/builder/build.cpp
    ${EXTERNAL_DIR}/sshash/gz/zip_stream.cpp
)

add_library(sshash_static STATIC ${SSHASH_SOURCES})
target_link_libraries(links INTERFACE sshash_static)

# zlib + threads

target_link_libraries(links INTERFACE ZLIB::ZLIB)
target_link_libraries(links INTERFACE Threads::Threads)

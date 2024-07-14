# SSHash library

message(STATUS "SSHash will use a maximum kmer length of 63")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSSHASH_USE_MAX_KMER_LENGTH_63")

set(sshash_sources
  ${external_dir}/sshash/dictionary.cpp
  ${external_dir}/sshash/info.cpp
  ${external_dir}/sshash/builder/build.cpp
  ${external_dir}/sshash/gz/zip_stream.cpp
)

add_library(sshash STATIC ${sshash_sources})
list(APPEND deps_libs sshash)
list(APPEND deps sshash)
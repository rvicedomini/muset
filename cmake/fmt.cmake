# fmt library

ExternalProject_Add(FMT
  PREFIX ${external_bindir}/FMT
  SOURCE_DIR ${external_dir}/fmt
  CMAKE_ARGS -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=ON
             -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
  INSTALL_COMMAND ""
  LOG_CONFIGURE ON
  LOG_BUILD ON
)

list(APPEND includes ${external_dir}/fmt/include)
list(APPEND deps_libs ${external_bindir}/FMT/src/FMT-build/libfmt.a)
list(APPEND deps FMT)

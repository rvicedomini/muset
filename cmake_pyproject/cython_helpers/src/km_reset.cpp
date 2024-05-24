#include <iostream>
#include <unistd.h>
#include <Python.h>

void km_reset() {
    optind = 1;   // Reset getopt's internal index variable
    #ifdef __GLIBC__
        optopt = 0;   // Optional, to be extra thorough
        optarg = nullptr; // Optional, to reset any arguments
    #endif
}

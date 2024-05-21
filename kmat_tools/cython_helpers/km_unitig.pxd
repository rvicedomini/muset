#cython: language_level=3

cdef extern from "lib/include/km_unitig.h":
    int main_unitig(int argc, char **argv) nogil
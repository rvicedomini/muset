#cython: language_level=3

cdef extern from "lib/include/km_basic_filter.h":
    int main_basic_filter(int argc, char **argv) nogil
#cython: language_level=3

cdef extern from "km_reverse.cpp":
    int main_reverse(int argc, char **argv) nogil
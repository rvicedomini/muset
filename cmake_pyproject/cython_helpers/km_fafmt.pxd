#cython: language_level=3

cdef extern from "km_fafmt.cpp":
    int main_fafmt(int argc, char **argv) nogil
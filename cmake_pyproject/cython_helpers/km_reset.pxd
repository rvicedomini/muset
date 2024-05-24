#cython: language_level=3
# km_reset.pxd

cdef extern from "include/km_reset.cpp":
    void km_reset()
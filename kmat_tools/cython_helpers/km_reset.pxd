#cython: language_level=3
# km_reset.pxd

cdef extern from "lib/include/km_reset.h":
    void km_reset()
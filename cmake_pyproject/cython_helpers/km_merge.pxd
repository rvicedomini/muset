#cython: language_level=3

# Declare main_merge function from C
cdef extern from "km_merge.cpp":
    int main_merge(int argc, char **argv)

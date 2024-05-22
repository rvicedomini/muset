#cython: language_level=3

# Declare main_fasta function from C
cdef extern from "km_fasta.cpp":
    int main_fasta(int argc, char **argv)

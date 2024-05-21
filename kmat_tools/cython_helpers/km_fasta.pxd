#cython: language_level=3

# Declare main_fasta function from C
cdef extern from "lib/include/km_fasta.h":
    int main_fasta(int argc, char **argv)

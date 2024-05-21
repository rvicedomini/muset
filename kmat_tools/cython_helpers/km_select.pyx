# km_select.pyx
from libc.stdlib cimport abort, calloc, free
from libc.stdio cimport fopen, fclose, fputs, stderr, stdout
from libc.string cimport strcmp
from cpython.mem cimport PyMem_Malloc, PyMem_Free

cdef extern from "common.h":
    bint next_kmer(char *, int, FILE *)
    bint next_kmer_and_line(char *, int, char **, size_t *, FILE *)
    int ktcmp(const char *, const char *)

def main_select(int ksize=31, char *out_fname=None, bint do_select=True, bint use_ktcmp=False):
    cdef:
        FILE *selfile
        FILE *matfile
        FILE *outfile
        char *sel_kmer = <char *>calloc(ksize+1,1)
        char *mat_kmer = <char *>calloc(ksize+1,1)
        char *line = NULL
        size_t line_size = 0
        bint ret_sel, ret_mat
        size_t tot_kmers, kept_kmers = 0
        int ret_cmp

    selfile = fopen("matrix_1","r")
    if selfile == NULL:
        print(f"Cannot open file matrix_1")
        return 1

    matfile = fopen("matrix_2","r")
    if matfile == NULL:
        print(f"Cannot open file matrix_2")
        fclose(selfile)
        return 1

    outfile = fopen(out_fname,"w") if out_fname else stdout
    if outfile != stdout and outfile == NULL:
        print(f"Cannot open output file {out_fname}")
        fclose(selfile)
        fclose(matfile)
        return 1

    ret_sel = next_kmer(sel_kmer, ksize, selfile)
    ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile)
    tot_kmers = ret_mat
    while ret_sel and ret_mat:
        ret_cmp = ktcmp(sel_kmer,mat_kmer) if use_ktcmp else strcmp(sel_kmer, mat_kmer)
        if ret_cmp == 0:
            if do_select:
                fputs(line,outfile)
                kept_kmers += 1
            ret_sel = next_kmer(sel_kmer, ksize, selfile)
            ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile)
            tot_kmers += ret_mat
        elif ret_cmp < 0:
            ret_sel = next_kmer(sel_kmer, ksize, selfile)
        else: # ret_cmp > 0
            if not do_select:
                fputs(line,outfile)
                kept_kmers += 1
            ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile)
            tot_kmers += ret_mat

    while ret_mat:
        if not do_select:
            fputs(line,outfile)
            kept_kmers += 1
        ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile)
        tot_kmers += ret_mat

    print(f"[info] {tot_kmers}\ttotal k-mers")
    print(f"[info] {kept_kmers}\tretained k-mers")

    free(line)
    fclose(selfile)
    fclose(matfile)
    if outfile != stdout:
        fclose(outfile)

    return 0
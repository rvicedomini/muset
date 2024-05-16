#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

#include "common.h"


int main_select(int argc, char **argv) {

  int ksize = 31;
  char *out_fname = NULL;
  bool do_select = true, use_ktcmp = false, help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "k:o:vzh")) != -1) {
    switch (c) {
      case 'k':
        ksize = strtol(optarg, NULL, 10);
        break;
      case 'o':
        out_fname = optarg;
        break;
      case 'v':
        do_select = false;
        break;
      case 'z':
        use_ktcmp = true;
        break;
      case 'h':
        help_opt = true;
        break;
      case '?':
        return 1;
      default:
        abort();
    }
  }

  if(ksize <= 0) { 
    fprintf(stderr, "Invalid value of k: %d\n",ksize);
    return 1;
  }

  if(argc-optind != 2 || help_opt) {
    fprintf(stdout, "Usage: kmtools select [options] <matrix_1> <matrix_2>\n\n");
    fprintf(stdout, "Select lines from <matrix_2> corresponding to k-mers belonging to <matrix_1>.\n");
    fprintf(stdout, "Input matrices are assumed to be sorted by k-mer.\n\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -k INT   size of k-mers in the input matrices [31]\n");
    fprintf(stdout, "  -o FILE  output matrix to FILE [stdout]\n");
    fprintf(stdout, "  -v       select k-mers that DO NOT belong to <matrix_1>\n");
    fprintf(stdout, "  -z       use kmtricks order of nucleotides: A<C<T<G\n");
    fprintf(stdout, "  -h       print this help message\n");
    return 0;
  }

  FILE *selfile = fopen(argv[optind],"r");
  if(selfile == NULL) {
    fprintf(stderr,"Cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *matfile = fopen(argv[optind+1],"r");
  if(matfile == NULL) {
    fprintf(stderr,"Cannot open file \"%s\"\n",argv[optind+1]);
    fclose(selfile); 
    return 1;
  }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    fprintf(stderr,"Cannot open output file \"%s\"\n",out_fname);
    fclose(selfile);
    fclose(matfile);
    return 1;
  }

  char *sel_kmer = (char *)calloc(ksize+1,1);
  char *mat_kmer = (char *)calloc(ksize+1,1);
  char *line = NULL;
  size_t line_size = 0;

  bool ret_sel = next_kmer(sel_kmer, ksize, selfile);
  bool ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
  size_t tot_kmers = ret_mat, kept_kmers = 0;
  while(ret_sel && ret_mat){
    int ret_cmp = use_ktcmp ? ktcmp(sel_kmer,mat_kmer) : strcmp(sel_kmer, mat_kmer);
    if(ret_cmp == 0) {
      if(do_select){ fputs(line,outfile); kept_kmers++; }
      ret_sel = next_kmer(sel_kmer, ksize, selfile);
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
      tot_kmers += ret_mat;
    } else if(ret_cmp < 0) {
      ret_sel = next_kmer(sel_kmer, ksize, selfile);
    } else { // ret_cmp > 0
      if(!do_select){ fputs(line,outfile); kept_kmers++; }
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
      tot_kmers += ret_mat;
    }
  }
  
  // output possibly remaining k-mers
  while(ret_mat) {
    if(!do_select) { fputs(line,outfile); kept_kmers++; }
    ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
    tot_kmers += ret_mat;
  }

  fprintf(stderr, "[info] %lu\ttotal k-mers\n", tot_kmers);
  fprintf(stderr, "[info] %lu\tretained k-mers\n", kept_kmers);

  free(line);
  fclose(selfile);
  fclose(matfile);
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

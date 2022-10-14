#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>


char* next_kmer(char *kmer, int ksize, FILE *stream) {

  int c;

  // read first ksize characters in buf
  for(int i=0; i<ksize; i++) {
    c = getc(stream);
    if(!isalpha(c)) { return NULL; }
    kmer[i] = c;
  }

  // discard following characters until the end of line or EOF
  c = getc(stream);
  while(c != '\n' && c != EOF) { c = getc(stream); }

  return kmer;
}

bool next_kmer_and_line(char *kmer, int ksize, char **line, size_t *line_size, FILE *stream) {

  if(getline(line, line_size, stream) < ksize) { return false; }

  // read first ksize characters in buf
  for(int i=0; i<ksize; i++) {
    if(!isalpha((*line)[i])) { return false; }
    kmer[i] = (*line)[i];
  }

  return true;
}

const uint8_t n2kt[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

bool ktcmp(char *k1, char *k2) {
  while(*k1 && (*k1 == *k2)){ k1++; k2++; }
  return n2kt[*(unsigned char *)k1] - n2kt[*(unsigned char *)k2];
}


int main(int argc, char **argv) {

  int ksize = 31;
  char *out_fname = NULL;
  bool do_select = true, use_ktcmp = false, help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "k:o:vh")) != -1) {
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
        use_ktcmp = false;
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
    fprintf(stdout, "Usage: km_select [options] <matrix_1> <matrix_2>\n\n");
    fprintf(stdout, "Select lines from <matrix_2> corresponding to k-mers belonging to <matrix_1>.\n");
    fprintf(stdout, "Input matrices are assumed to be sorted by k-mer.\n\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -k INT   size of k-mers in the input matrices [31]\n");
    fprintf(stdout, "  -o FILE  output reverse-complement matrix to FILE [stdout]\n");
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
  while(ret_sel && ret_mat){
    int ret_cmp = use_ktcmp ? ktcmp(sel_kmer,mat_kmer) : strcmp(sel_kmer, mat_kmer);
    if(ret_cmp == 0) {
      if(do_select){ fputs(line,outfile); }
      ret_sel = next_kmer(sel_kmer, ksize, selfile);
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
    } else if(ret_cmp < 0) {
      ret_sel = next_kmer(sel_kmer, ksize, selfile);
    } else { // ret_cmp > 0
      if(!do_select){ fputs(line,outfile); }
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
    }
  }

  free(line);
  fclose(selfile);
  fclose(matfile);
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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


int main(int argc, char **argv) {

  int ksize = 31;
  char *out_fname = NULL;
  bool do_select = true;

  int c;
  while ((c = getopt(argc, argv, "k:o:v")) != -1) {
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

  if(argc-optind != 2) {
    fprintf(stderr, "Usage: %s [options] kmer_file matrix_file\n", argv[0]);
    return 1;
  }

  FILE *kfile = fopen(argv[optind],"r");
  if(kfile == NULL) { fprintf(stderr,"Cannot open file \"%s\"\n",argv[optind]); return 1; }

  FILE *matfile = fopen(argv[optind+1],"r");
  if(matfile == NULL) { fclose(kfile); fprintf(stderr,"Cannot open file \"%s\"\n",argv[optind+1]); return 1; }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) { fclose(kfile); fclose(matfile); fprintf(stderr,"Cannot open output file \"%s\"\n",out_fname); return 1; }

  char *sel_kmer = (char *)calloc(ksize+1,1);
  char *mat_kmer = (char *)calloc(ksize+1,1);
  char *line = NULL;
  size_t line_size = 0;

  long mat_count = 0;

  bool ret_sel = next_kmer(sel_kmer, ksize, kfile);
  bool ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
  while(ret_sel && ret_mat){
    int ret_cmp = strcmp(sel_kmer, mat_kmer);
    if(ret_cmp == 0) {
      if(do_select){ fputs(line,outfile); }
      ret_sel = next_kmer(sel_kmer, ksize, kfile);
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
      if(++mat_count % 16000000 == 0) { fprintf(stderr,"%ld processed\n",mat_count); }
    } else if(ret_cmp < 0) {
      ret_sel = next_kmer(sel_kmer, ksize, kfile);
    } else { // ret_cmp > 0
      if(!do_select){ fputs(line,outfile); }
      ret_mat = next_kmer_and_line(mat_kmer, ksize, &line, &line_size, matfile);
      if(++mat_count % 16000000 == 0) { fprintf(stderr,"%ld processed\n",mat_count); }
    }
  }

  free(line);
  fclose(kfile);
  fclose(matfile);
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

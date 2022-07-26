#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>


int main(int argc, char **argv) {

  int min_count = 5;
  char *out_fname = NULL;

  int c;
  while ((c = getopt(argc, argv, "c:o:")) != -1) {
    switch (c) {
      case 'c':
        min_count = strtol(optarg, NULL, 10);
        break;
      case 'o':
        out_fname = optarg;
        break;
      case '?':
        return 1;
      default:
        abort();
    }
  }

  if(argc-optind != 1) {
    fprintf(stderr, "Usage: %s [-c INT] [-o OUTPUT_FILE] MATRIX_FILE\n", argv[0]);
    return 1;
  }

  if(min_count <= 0) { 
    fprintf(stderr, "[error] -c parameter should be an integer > 0\n", min_count);
    return 1;
  }

  FILE *matfile = fopen(argv[optind],"r");
  if(matfile == NULL) { 
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    fclose(matfile);
    fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname); return 1; }

  size_t n_samples = 0, n_kmers = 0, n_retrieved = 0;

  char *line = NULL, *line_cpy = NULL;
  size_t line_size = 0, line_cpy_size = 0;
  ssize_t ch_read = getline(&line, &line_size, matfile);
  while(ch_read >= 0) {

    if(line_cpy_size < line_size) {
      line_cpy_size = line_size;
      line_cpy = (char*)realloc(line_cpy,line_cpy_size);
    }
    line_cpy = strcpy(line_cpy,line);

    char *elem = strtok(line," \t\n");
    if(elem == NULL){ continue; } // skip empty lines
    ++n_kmers;

    size_t n_zeros = 0, n_present = 0;
    while((elem = strtok(NULL," \t\n")) != NULL) {
      if(n_kmers == 1){ 
        ++n_samples; 
      }
      long val = strtol(elem,NULL,10);
      if(val == 0){ ++n_zeros; } else if(val >= min_count){ ++n_present; }
    }

    if(n_zeros >= 10 && n_present >= 10) {
      ++n_retrieved;
      fputs(line_cpy, outfile);
    }

    if((n_kmers & ((1U<<20)-1)) == 0) {
      fprintf(stderr, "%u k-mers processed, %u retrieved\n", n_kmers, n_retrieved);
    }
    ch_read = getline(&line, &line_size, matfile);
  }

  fprintf(stderr, "%u\tsamples\n", n_samples);
  fprintf(stderr, "%u\tk-mers\n", n_kmers);
  fprintf(stderr, "%u\tk-mers that are absent from at least 10 samples but present (abund >= %d) in at least 10 others\n", n_retrieved, min_count);

  free(line); 
  free(line_cpy);
  fclose(matfile);
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

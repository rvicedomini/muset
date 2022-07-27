#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>


int main(int argc, char **argv) {

  int min_samples = 10, min_abund = 10;
  char *out_fname = NULL;
  bool help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "a:c:o:h")) != -1) {
    switch (c) {
      case 'a':
        min_abund = strtol(optarg, NULL, 10);
        break;
      case 'c':
        min_samples = strtol(optarg, NULL, 10);
        break;
      case 'o':
        out_fname = optarg;
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

  bool valid_opts = true;
  if(min_abund <= 0) {
    valid_opts = false;
    fprintf(stderr, "[error] -a should be a positive integer.\n");
  }
  if(min_samples <= 0) {
    valid_opts = false;
    fprintf(stderr, "[error] -c should be a positive integer.\n");
  }
  if(!valid_opts) {
    return 1;
  }

  if(argc-optind != 1 || help_opt) {
    fprintf(stdout, "Usage: km_basic_filter [options] <in.mat>\n\n");

    fprintf(stdout, "Filter a matrix by selecting k-mers that are potentially differential.\n");
    
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -a INT   min abundance to define a k-mer as present in a sample [10]\n");
    fprintf(stdout, "  -c INT   min number of samples for which a k-mer should be present/absent [10]\n");
    fprintf(stdout, "  -o FILE  output filtered matrix to FILE [stdout]\n");
    fprintf(stdout, "  -h       print this help message\n");
    return 0;
  }

  FILE *matfile = strcmp(argv[optind],"-") ? fopen(argv[optind],"r") : stdin;
  if(matfile == NULL) { 
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    if(matfile != stdin){ fclose(matfile); }
    fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname);
    return 1;
  }

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
      if(val == 0){ ++n_zeros; } else if(val >= min_abund){ ++n_present; }
    }

    if(n_zeros >= min_samples && n_present >= min_samples) {
      ++n_retrieved;
      fputs(line_cpy, outfile);
    }

    if((n_kmers & ((1U<<20)-1)) == 0) {
      fprintf(stderr, "%lu k-mers processed, %lu retrieved\n", n_kmers, n_retrieved);
    }
    ch_read = getline(&line, &line_size, matfile);
  }

  fprintf(stderr, "[info] %lu\tsamples\n", n_samples);
  fprintf(stderr, "[info] %lu\tk-mers\n", n_kmers);
  fprintf(stderr, "[info] %lu\tretained k-mers (present/absent in >= %d samples, abundance >= %d)\n", n_retrieved, min_samples, min_abund);

  free(line); free(line_cpy);
  if(matfile != stdin){ fclose(matfile); }
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

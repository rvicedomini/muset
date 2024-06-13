#include "common.h"

int main_basic_filter(int argc, char **argv) {

  size_t min_zeros=10, min_nz=10, min_abund=1;
  double min_zero_frac=0.5, min_nz_frac=0.1;
  char *out_fname = NULL;
  bool verbose_opt=false, help_opt=false;
  
  bool min_zero_frac_opt=false, min_nz_frac_opt=false;

  int c;
  while ((c = getopt(argc, argv, "a:f:F:n:N:o:vh")) != -1) {
    switch (c) {
      case 'a':
        min_abund = strtoul(optarg, NULL, 10);
        break;
      case 'n':
        min_zeros = strtoul(optarg, NULL, 10);
        break;
      case 'N':
        min_nz = strtoul(optarg, NULL, 10);
        break;
      case 'o':
        out_fname = optarg;
        break;
      case 'f':
        min_zero_frac_opt = true;
        min_zero_frac = atof(optarg);
        break;
      case 'F':
        min_nz_frac_opt = true;
        min_nz_frac = atof(optarg);
        break;
      case 'v':
        verbose_opt = true;
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

  if(min_zero_frac_opt && (min_zero_frac < 0.01 || min_zero_frac >0.99)) {
    fprintf(stderr, "[error] -f must be in the [0.01,0.99] interval.\n");
    return 1;
  }
  if(min_nz_frac_opt && (min_nz_frac < 0.01 || min_nz_frac > 0.95)) {
    fprintf(stderr, "[error] -F must be in the [0.01,0.95] interval.\n");
    return 1;
  }

  if(argc-optind != 1 || help_opt) {
    fprintf(stdout, "Usage: kmat_tools filter [options] <in.mat>\n\n");

    fprintf(stdout, "Filter a matrix by selecting k-mers that are potentially differential.\n\n");
    
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -a INT    min abundance to define a k-mer as present in a sample [1]\n");
    fprintf(stdout, "  -n INT    min number of samples for which a k-mer should be absent [10]\n");
    fprintf(stdout, "  -f FLOAT  fraction of samples for which a k-mer should be absent (overrides -n)\n");
    fprintf(stdout, "  -N INT    min number of samples for which a k-mer should be present [10]\n");
    fprintf(stdout, "  -F FLOAT  fraction of samples for which a k-mer should be present (overrides -N)\n");
    fprintf(stdout, "  -o FILE   output filtered matrix to FILE [stdout]\n");
    fprintf(stdout, "  -v        verbose output\n");
    fprintf(stdout, "  -h        print this help message\n");
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
      size_t val = strtol(elem,NULL,10);
      if(val >= min_abund){ ++n_present; } else { ++n_zeros; }
    }

    bool enough_zeros = (min_zero_frac_opt && n_zeros >= min_zero_frac*n_samples) || (!min_zero_frac_opt && n_zeros >= min_zeros);
    bool enough_nz = (min_nz_frac_opt && n_present >= min_nz_frac*n_samples) || (!min_nz_frac_opt && n_present >= min_nz);
    if(enough_zeros && enough_nz) {
      ++n_retrieved;
      fputs(line_cpy, outfile);
    }

    if(verbose_opt && (n_kmers & ((1U<<20)-1)) == 0) {
      fprintf(stderr, "%lu k-mers processed, %lu retrieved\n", n_kmers, n_retrieved);
    }
    ch_read = getline(&line, &line_size, matfile);
  }

  fprintf(stderr, "[info] %lu\tsamples\n", n_samples);
  fprintf(stderr, "[info] %lu\ttotal k-mers\n", n_kmers);
  fprintf(stderr, "[info] %lu\tretained k-mers\n", n_retrieved);

  free(line); free(line_cpy);
  if(matfile != stdin){ fclose(matfile); }
  if(outfile != stdout){ fclose(outfile); }

  return 0;
}

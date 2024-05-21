#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include <zlib.h>

#include "kseq.h"
KSEQ_INIT(gzFile, gzread)

#include "khash.h"

KHASH_MAP_INIT_STR(str, const char *)
KHASH_MAP_INIT_STR(cnt, size_t)
KHASH_MAP_INIT_STR(vec, uint32_t *)

#include "common.h"


int main_unitig(int argc, char **argv) {

  int ksize = 31;
  size_t utg_min_length = 0;
  char *out_fname = NULL;
  bool help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "k:l:o:h")) != -1) {
    switch (c) {
      case 'k':
        ksize = strtol(optarg, NULL, 10);
        break;
      case 'l':
        utg_min_length = strtoul(optarg, NULL, 10);
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

  if(ksize <= 0) { 
    fprintf(stderr, "Invalid value of k: %d\n",ksize);
    return 1;
  }

  if(argc-optind != 2 || help_opt) {
    fprintf(stdout, "Usage: km_unitig [options] <unitigs.fasta> <kmer_matrix>\n\n");
    fprintf(stdout, "Creates a unitig matrix.\n\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  -k INT   size of k-mers of input matrices [31]\n");
    fprintf(stdout, "  -l INT   minimum length of unitigs to consider [0]\n");
    fprintf(stdout, "  -o FILE  write unitig matrix to FILE [stdout]\n");
    fprintf(stdout, "  -h       print this help message\n");
    return 0;
  }

  gzFile utg = gzopen(argv[optind],"r");
  if(utg == NULL) {
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind]);
    return 1;
  }

  FILE *mat = fopen(argv[optind+1],"r");
  if(mat == NULL) {
    fprintf(stderr,"[error] cannot open file \"%s\"\n",argv[optind+1]);
    gzclose(utg); 
    return 1;
  }

  // process unitig file
  
  kh_cnt_t *utg2len = kh_init(cnt);
  kh_str_t *kmer2utg = kh_init(str);

  int64_t l = 0;
  kseq_t *seq = kseq_init(utg);
  while ((l = kseq_read(seq)) >= 0) {
    if (seq->seq.l < utg_min_length) {
      continue;
    }

    khint_t it = kh_get(cnt, utg2len, seq->name.s);
    if (it == kh_end(utg2len)) {
      char *s = strdup(seq->name.s);
      int ret; it = kh_put(cnt, utg2len, s, &ret);
    }
    const char *utg_name = kh_key(utg2len, it);
    kh_value(utg2len, it) = seq->seq.l >= ksize ? seq->seq.l - ksize + 1 : 0;

    // add kmers of seq->seq.s to kmer2utg
    for(int i=0; i+ksize <= seq->seq.l; ++i) {
      char *kmer = calloc(ksize+1,1); memcpy(kmer, seq->seq.s+i, ksize);
      km_canonicalize(kmer,ksize);
      int ret; khint_t k = kh_put(str, kmer2utg, kmer, &ret);
      if (ret == 0) { // kmer already present in the hash table (should not happen with unitigs)
        fprintf(stderr,"[error] kmer \"%s\" present more than once in the unitig sequences\n", kmer);
        kseq_destroy(seq);
        gzclose(utg);
        fclose(mat);
        return 1;
      }
      kh_value(kmer2utg,k) = utg_name;
    }
  }
  fprintf(stderr,"[info] unitigs: %u\n", kh_size(utg2len));
  kseq_destroy(seq);
  gzclose(utg);

  FILE *outfile = out_fname ? fopen(out_fname,"w") : stdout;
  if(outfile != stdout && outfile == NULL) {
    fprintf(stderr,"[error] cannot open output file \"%s\"\n",out_fname);
    fclose(mat);
    return 1;
  }

  // process matrix lines

  kh_vec_t *utg_samples = kh_init(vec);
  char *kmer = (char *)calloc(ksize+1,1);
  char *line = NULL;
  size_t line_size = 0;
  size_t line_count = 0;

  bool has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
  size_t n_samples = has_kmer ? samples_number(line) : 0;
  fprintf(stderr,"[info] samples: %lu\n", n_samples);

  while(has_kmer) {
    line_count++;
    km_canonicalize(kmer,ksize);
    
    khint_t k = kh_get(str, kmer2utg, kmer);
    if (k == kh_end(kmer2utg)) {
      has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
      continue;
    }

    const char *utg_name = kh_val(kmer2utg,k);
    k = kh_get(vec, utg_samples, utg_name);
    if (k == kh_end(utg_samples)) {
      int ret; k = kh_put(vec, utg_samples, utg_name, &ret);
      kh_value(utg_samples, k) = (uint32_t *)calloc(2*n_samples, sizeof(uint32_t));
    }
    
    uint32_t *counts = kh_value(utg_samples, k);
    char *start = second_column(line);
    int c = 0; char *tok = strtok(start," \t\n");
    while(tok) {
      uint32_t num = strtoul(tok,NULL,10);
      counts[c] += (num > 0);
      counts[c+1] += num;
      tok = strtok(NULL," \t\n");
      c += 2;
    }

    has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
  }

  free(kmer);
  free(line);
  fclose(mat);

  // write output
  fprintf(stderr,"[info] writing output\n");

  for (khiter_t k = kh_begin(utg2len); k != kh_end(utg2len); ++k) {

    if (!kh_exist(utg2len, k)) { continue; }

    const char *utg_name = kh_key(utg2len,k);
    fprintf(outfile, "%s", utg_name);

    khiter_t it = kh_get(vec, utg_samples, utg_name);
    if (it == kh_end(utg_samples)) {
      for(int i=0; i < n_samples; ++i) {
        fprintf(outfile, " 0");
      }
      fprintf(outfile, "\n");
      continue;
    }

    size_t utg_size = kh_value(utg2len, k);
    uint32_t *counts = kh_value(utg_samples, it);
    for(int i=0; i < 2*n_samples; i+=2) {
      double frac = (1.0 * counts[i])/utg_size;
      double avg_coverage = (1.0 * counts[i+1])/utg_size;
      fprintf(outfile, " %.2f;%.2f", avg_coverage, frac);
    }
    fprintf(outfile,"\n");
  }
  
  if(outfile != stdout){ fclose(outfile); }

  // free allocated memory

  for (khiter_t it = kh_begin(utg_samples); it != kh_end(utg_samples); ++it) {
    if (kh_exist(utg_samples, it)) {
        uint32_t *samples = kh_val(utg_samples,it);
        free(samples);
    }
  }
  kh_destroy(vec, utg_samples);

  for (khiter_t it = kh_begin(kmer2utg); it != kh_end(kmer2utg); ++it) {
    if (kh_exist(kmer2utg, it)) {
        const char *kmer = kh_key(kmer2utg,it);
        free((char*)kmer);
    }
  }
  kh_destroy(str, kmer2utg);

  for (khiter_t k = kh_begin(utg2len); k != kh_end(utg2len); ++k) {
    if (kh_exist(utg2len, k)) {
        const char *utg_name = kh_key(utg2len,k);
        free((char*)utg_name);
    }
  }
  kh_destroy(cnt, utg2len);

  return 0;
}

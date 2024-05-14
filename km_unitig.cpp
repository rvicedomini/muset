#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>
#include <unistd.h>
#include <filesystem>
#include <unordered_map>

#include "include/kseq++/seqio.hpp"
#include "include/ankerl/unordered_dense.h"
#include "common.h"


int main_unitig(int argc, char **argv) {

  std::size_t ksize = 31;
  std::size_t utg_min_length = 0;
  std::string out_fname;
  bool help_opt = false;

  int c;
  while ((c = getopt(argc, argv, "k:l:o:h")) != -1) {
    switch (c) {
      case 'k':
        ksize = std::strtoul(optarg, NULL, 10);
        break;
      case 'l':
        utg_min_length = std::strtoul(optarg, NULL, 10);
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

  if(argc-optind != 2 || help_opt) {
    std::cout << "Usage: km_unitig [options] <unitigs.fasta> <kmer_matrix>\n\n";
    std::cout << "Creates a unitig matrix.\n\n";
    std::cout << "Options:\n";
    std::cout << "  -k INT   size of k-mers of input matrices [31]\n";
    std::cout << "  -l INT   minimum length of unitigs to consider [0]\n";
    std::cout << "  -o FILE  write unitig matrix to FILE [stdout]\n";
    std::cout << "  -h       print this help message\n";
    return 0;
  }

  std::string utg_file = argv[optind];
  if(!std::filesystem::exists(utg_file.c_str())) {
    std::cerr << "[error] unitig file \"" << utg_file << "\" does not exist\n";
    return 1;
  }

  std::string mat_file = argv[optind+1];
  if(!std::filesystem::exists(mat_file.c_str())) {
    std::cerr << "[error] matrix file \"" << mat_file << "\" does not exist\n";
    return 1;
  }

  if(ksize <= 0) { 
    std::cerr << "[error] -k parameter must be greater than zero\n";
    return 1;
  }

  // print some info to stderr

  std::cerr << "[info] k-mer size: " << ksize << std::endl;

  // process unitig file

  std::vector<std::string> utg_names;
  std::vector<std::size_t> utg_size;
  ankerl::unordered_dense::segmented_map<std::string, std::size_t> kmer2utg;
  
  klibpp::KSeq record;
  klibpp::SeqStreamIn utg_ssi(utg_file.c_str());
  while (utg_ssi >> record) {
    if (record.seq.length() < utg_min_length) {
        continue;
    }

    std::size_t utg_id = utg_names.size();
    std::size_t utg_len = record.seq.length();
    utg_size.push_back(utg_len >= ksize ? utg_len - ksize + 1 : 0);
    utg_names.push_back(record.name);

    const char *seq = record.seq.c_str();
    for(std::size_t i=0; i+ksize <= record.seq.length(); ++i) {
        std::string kmer(seq+i, ksize);
        canonicalize(kmer);

        auto it = kmer2utg.find(kmer);
        if(it == kmer2utg.end()) {
            kmer2utg[kmer] = utg_id;
        } else {
            std::cerr << "[error] kmer \"" << it->first << "\" was previously inserted from \"" << utg_names[it->second] << "\", a duplicate is found in \"" << utg_names[utg_id] << "\"" << std::endl;
            return 1;
        }
    }
  }

  std::cerr << "[info] unitigs processed: " << utg_names.size() << std::endl;
  std::cerr << "[info] k-mer processed: " << kmer2utg.size() << std::endl;

  // process matrix file

  typedef std::pair<uint32_t,uint32_t> sample_t;
  ankerl::unordered_dense::segmented_map<std::size_t, std::vector<sample_t>> utg_samples;

  FILE *mat = fopen(mat_file.c_str(),"r");
  if(mat == NULL) {
    std::cerr << "[error] cannot open matrix file \"" << mat_file <<"\"\n";
    return 1;
  }

  char *kmer = (char *)calloc(ksize+1,1);
  char *line = NULL;
  size_t line_size = 0;
  size_t line_count = 0;

  bool has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
  std::size_t n_samples = has_kmer ? samples_number(line) : 0;
  fprintf(stderr,"[info] samples: %lu\n", n_samples);

  while(has_kmer) {
    line_count++;
    canonicalize(kmer,ksize);

    auto it = kmer2utg.find(std::string(kmer));
    if (it == kmer2utg.end()) {
        has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
        continue;
    }

    std::size_t utg_id = it->second;
    auto it2 = utg_samples.find(utg_id);
    if(it2 == utg_samples.end()) {
        utg_samples[utg_id] = std::vector<sample_t>(n_samples);
    }

    auto& counts = utg_samples[utg_id];
    char *start = second_column(line);
    int c = 0; char *tok = strtok(start," \t\n");
    while(tok) {
      uint32_t num = strtoul(tok,NULL,10);
      counts[c].first += (num > 0 ? 1 : 0);
      counts[c].second += num;
      tok = strtok(NULL," \t\n");
      c++;
    }

    has_kmer = next_kmer_and_line(kmer, ksize, &line, &line_size, mat);
  }

  free(kmer);
  free(line);
  fclose(mat);

  std::ostream* fp = &std::cout;
  std::ofstream fout;
  if(!out_fname.empty()) {
    fout.open(out_fname.c_str());
    if(!fout.good()) {
        std::cerr << "[error] cannot open output file \"" << out_fname << "\"\n";
        return 1;
    }
    fp = &fout;
  }

  *fp << std::fixed << std::setprecision(2);

  // write output
  std::cerr << "[info] writing output" << std::endl;

  for(std::size_t utg_id=0; utg_id < utg_names.size(); utg_id++) {
    std::string& utg_name = utg_names[utg_id];
    std::size_t utg_n_kmers = utg_size[utg_id] ;

    *fp << utg_name;

    auto it = utg_samples.find(utg_id);
    if (it == utg_samples.end()) {
        for(std::size_t i=0; i < n_samples; ++i) { *fp <<  ' ' << 0.0 << ';' << 0.0; }
        *fp << '\n';
        continue;
    }

    auto& counts = utg_samples[utg_id];
    for(auto p : counts) {
        double frac = (1.0 * p.first)/utg_n_kmers;
        double avg_coverage = (1.0 * p.second)/utg_n_kmers;
        *fp << ' ' << avg_coverage << ';' << frac;
    }

    *fp << '\n';
  }

  return 0;
}

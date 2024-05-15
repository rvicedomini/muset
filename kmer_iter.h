#pragma once

#include "include/kseq++/seqio.hpp"
#include "include/pthash/pthash.hpp"

struct PTHasher {
    typedef pthash::hash128 hash_type;
    static inline pthash::hash128 hash(const std::string& val, uint64_t seed) {
        return {pthash::MurmurHash2_64(val.c_str(), val.size(), seed),
                pthash::MurmurHash2_64(val.c_str(), val.size(), ~seed)};
    }
};

using pthash_mphf_t = pthash::single_phf<PTHasher, pthash::dictionary_dictionary, true>;

class kmer_fasta_iter : std::forward_iterator_tag {

    public:
        
        using value_type = std::string; // typedef lphash::kmer_t value_type;

        kmer_fasta_iter(klibpp::SeqStreamIn& ssi, size_t ksize);
        
        const kmer_fasta_iter& operator++();
        const std::string operator*() const { return m_kmer; }

        inline bool has_next() const { return m_has_next; };

    private:

        klibpp::SeqStreamIn& m_ssi;
        klibpp::KSeq m_record;
        std::size_t m_index;

        std::size_t m_ksize;
        std::string m_kmer;
        bool m_has_next;

        uint64_t* ref_count;
        
        friend bool operator==(const kmer_fasta_iter& a, const kmer_fasta_iter& b) { return a.m_has_next == b.m_has_next; };
        friend bool operator!=(const kmer_fasta_iter& a, const kmer_fasta_iter& b) { return a.m_has_next != b.m_has_next; };
};

kmer_fasta_iter::kmer_fasta_iter(klibpp::SeqStreamIn& ssi, std::size_t ksize) 
    : m_ssi(ssi), m_index(0), m_ksize(ksize), m_has_next(false) {
    
    if (m_ssi.fail()) {
        throw std::runtime_error("[kmer_fasta_iter] cannot open file");
    }

    if (m_ssi >> m_record) {
        m_has_next = true;
        operator++();
    }
}

const kmer_fasta_iter& kmer_fasta_iter::operator++() {

    while (m_has_next && (m_index + m_ksize) > m_record.seq.length()) {
        m_index = 0;
        m_ssi >> m_record;
        if (!m_ssi) {
            m_has_next = false;
        }
    }

    m_kmer = m_record.seq.substr(m_index,m_ksize);
    m_index++;

    return *this;
}

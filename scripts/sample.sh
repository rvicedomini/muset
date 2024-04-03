for i in {1..10}; do
    if [ $i -lt 5 ]; then
        seqkit sample -p 0.1 -s 11 /pasteur/gaia/homes/cduitama/DRR162400_1.fastq.gz | seqkit head -n 10 > test/D$i.fasta
    else
        seqkit sample -p 0.1 -s 11 /pasteur/gaia/homes/cduitama/DRR162400_2.fastq.gz | seqkit head -n 10 > test/D$i.fasta
    fi
done


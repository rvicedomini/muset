#This folder is made to test reproducibility of kmtools pipeline

To produce the results in output_1 and output_2:

First install [ggcat](https://github.com/algbio/ggcat?tab=readme-ov-file#installation).

Then you can install `kmtools` via conda:

```
git clone https://github.com/camiladuitama/kmat_tools.git
cd kmat_tools
conda env create -n kmat_tools --file environment.yaml
conda deactivate
conda activate kmat_tools

````

Then I ran the following commands:

```

kmtools pipeline -o output_1 test/fof.txt
kmtools pipeline -o output_2 test/fof.txt

````

Notice that the files after running ggcat change considerably between output_1 and output_2 despite having been produced with the same running parameters and input data.


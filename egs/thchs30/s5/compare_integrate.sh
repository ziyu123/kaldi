#!/bin/bash

for i in baseline2; do
  ./compare_results.py challenge/$i/scoring_kaldi/test.txt \
                       challenge/$i/scoring_kaldi/test_filt.txt \
                       challenge/$i/${i}_compare_all.txt

  ./compare_results.py challenge/$i/scoring_ch/test.txt \
                       challenge/$i/scoring_ch/test_filt.txt \
                       challenge/$i/${i}_compare_ch.txt

  ./compare_results.py challenge/$i/scoring_en/test.txt \
                       challenge/$i/scoring_en/test_filt.txt \
                       challenge/$i/${i}_compare_en.txt

done

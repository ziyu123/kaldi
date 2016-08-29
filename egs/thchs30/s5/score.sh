#!/bin/bash
(
# 'text' in ref/ and challeng/*/
for i in 3_4; do
  local/raw_score.sh challenge/ref/ challenge/$i/
  local/raw_score_ch.sh challenge/ref/ challenge/$i/
  local/raw_score_en.sh challenge/ref/ challenge/$i/
done
) || exit 1

exit 0

#!/bin/bash

. ./path.sh

data_dir=data

pit-score-referred-wav --add-normalized-log-pitch=true --add-raw-log-pitch=false scp:$data_dir/wav.scp

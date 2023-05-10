#!/bin/sh

# Switch dataset in the config_local.sh file by calling the desired function

#################
#   BIGANN10M   #
#################
dataset_bigann10M() {
  BASE_PATH=/data/bigann/base10M.bin
  QUERY_FILE=/data/bigann/query.public.10K.u8bin
  GT_FILE=/data/bigann/gt10M.bin
  PREFIX=bigann_10m
  DATA_TYPE=uint8
  DIST_FN=l2
  B=0.3
  K=10
  DATA_DIM=128
  DATA_N=10000000
}


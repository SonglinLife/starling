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
  K=1
  DATA_DIM=128
  DATA_N=10000000
}

dataset_deep10M(){
  BASE_PATH=/data/deep/base10M.bin
  QUERY_FILE=/data/deep/query10K.bin
  GT_FILE=/data/deep/gt10M.bin
  PREFIX=deep_10m
  DATA_TYPE=float
  DIST_FN=l2
  B=0.224
  K=1
  DATA_DIM=96
  DATA_N=9990000
}
dataset_space10M(){
  BASE_PATH=/data/space/spacev10m_sample.i8bin
  QUERY_FILE=/data/space/spacev10k_query.i8bin
  GT_FILE=/data/space/gt10M.bin
  PREFIX=space_10m
  DATA_TYPE=int8
  DIST_FN=l2
  B=0.233
  K=1
  DATA_DIM=96
  DATA_N=9990000
}

#!/bin/bash

data_type=int8
dist_fn=l2
base_file=/data/space/spacev10m_sample.i8bin
query_file=/data/space/spacev10k_query.i8bin
gt_file=/data/space/gt10M.bin
K=1000

/data/starling/build/tests/utils/compute_groundtruth --dist_fn $dist_fn --data_type $data_type --base_file $base_file --query_file $query_file --gt_file $gt_file --K $K



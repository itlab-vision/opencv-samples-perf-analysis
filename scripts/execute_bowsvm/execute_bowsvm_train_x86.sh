#!/bin/bash

#SBATCH --time=3500


# Change WORK_DIR with all builds
WORK_DIR="/home/kustikova_v"
data_dir="${WORK_DIR}/data/bow_svm/images/cifar-10-batches-bin"

# 100 = C_SVC, 101 = NU_SVC (nu parameter is required)
svm_types=(100)

# 0 =  LINEAR, 1 = POLY (degree parameter is required), 2 = RBF, 3 = SIGMOID, 4 = CHI2, 5 = INTER
kernel_types=(0)

max_iter=100

vocabulary_sizes=(250 500 1000)

#  "ORB" "BRISK" "MSER" "AKAZE" "KAZE"
detector_types=("SIFT")
descriptor_types=("SIFT")

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector")

versions_2=("sequence" "openmp")

echo "Execute experiments"
IFS=""
for svm_type in ${svm_types[@]}; do
  for kernel_type in ${kernel_types[@]}; do
    for voc_size in ${vocabulary_sizes[@]}; do
      for d_idx in ${!detector_types[@]}; do
        detector_type=${detector_types[$d_idx]}
        descriptor_type=${descriptor_types[$d_idx]}
        for v1 in ${versions_1[@]}; do
          for v2 in ${versions_2[@]}; do
            echo "============================================"
            output_file="${svm_type}_${kernel_type}_${voc_size}_${detector_type}_${descriptor_type}_${v1}_${v2}"
            echo "Output file names: ${output_file}(.xml is a model, .yml is a vocabulary)"
            echo "Version type 1: ${v1}"
            echo "Version type 2: ${v2}"
            echo "Working directory: ${PWD}"
            bin_file="${WORK_DIR}/${build_name_prefix}-${v2}-${v1}/bin/bow_svm"

            if [ $v2 = "openmp" ]
            then
              export GOMP_CPU_AFFINITY="0-9"
              export OMP_NUM_THREADS=10
            fi

            echo "Command line: ${bin_file} train ${data_dir} cifar10 ${output_file}.xml ${svm_type} ${kernel_type} ${max_iter} ${output_file}.yml ${voc_size} ${detector_type} ${descriptor_type}"
            ${bin_file} train ${data_dir} cifar10 ${output_file}.xml ${svm_type} ${kernel_type} ${max_iter} ${output_file}.yml ${voc_size} ${detector_type} ${descriptor_type}

            if [ $v2 = "openmp" ]
            then
              unset GOMP_CPU_AFFINITY
              unset OMP_NUM_THREADS
            fi

            echo "============================================"
          done
        done
      done
    done
  done
done

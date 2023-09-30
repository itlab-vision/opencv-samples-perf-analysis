#!/bin/bash

# Change WORK_DIR with all builds
WORK_DIR="/home/sipeed"
data_dir="${WORK_DIR}/data/bow_svm/images/cifar-10-batches-bin"
model_dir="${WORK_DIR}/data/bow_svm/models"

# 100 = C_SVC, 101 = NU_SVC
svm_types=(100)

# 0 =  LINEAR, 1 = POLY, 2 = RBF, 3 = SIGMOID, 4 = CHI2, 5 = INTER
kernel_types=(0)


vocabulary_sizes=(250 500 1000)

detector_types=("SIFT")

descriptor_types=("SIFT")

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector128" "vector512")

versions_2=("sequence")

save_result="off"

echo "Execute experiments"
IFS=""
for svm_type in ${svm_types[@]}; do
  for kernel_type in ${kernel_types[@]}; do
    for voc_size in ${vocabulary_sizes[@]}; do
      for detector_type in ${detector_types[@]}; do
        for descriptor_type in ${descriptor_types[@]}; do
          for v1 in ${versions_1[@]}; do
            for v2 in ${versions_2[@]}; do
              echo "============================================"
              input_file="${svm_type}_${kernel_type}_${voc_size}_${detector_type}_${descriptor_type}_scalar_sequence"
              if [ $save_result = "on" ]
              then
                output_file="${svm_type}_${kernel_type}_${voc_size}_${detector_type}_${descriptor_type}_${v1}_${v2}_predictions.yml"
              fi
              echo "Input file names: ${input_file}(.xml is a model, .yml is a vocabulary)"
              echo "SVM type: ${svm_type}"
              echo "Kernel type: ${kernel_type}"
              echo "Version type 1: ${v1}"
              echo "Version type 2: ${v2}"
              echo "Working directory: ${PWD}"
              bin_file="${WORK_DIR}/${build_name_prefix}-${v1}-${v2}/bin/bow_svm"
              echo "bin: ${bin_file}"


              echo "Command line: ${bin_file} inference ${data_dir} cifar10 ${model_dir}/${input_file}.xml ${model_dir}/${input_file}.yml ${detector_type} ${descriptor_type} ${output_file}"
              ${bin_file} inference ${data_dir} cifar10 ${model_dir}/${input_file}.xml ${model_dir}/${input_file}.yml ${detector_type} ${descriptor_type} ${output_file}

              echo "============================================"
            done
          done
        done
      done
    done
  done
done

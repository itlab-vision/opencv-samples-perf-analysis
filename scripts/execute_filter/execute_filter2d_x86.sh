#!/bin/bash

#SBATCH --time=3500


# Change WORK_DIR with all builds
WORK_DIR="/home/kustikova_v"
data_dir="${WORK_DIR}/data/filter"

# Change SAVE_OUTPUT option
SAVE_OUTPUT=false

input_images=(
"1920_1080.png"
"3840_2160.bmp"
"7680_4320.bmp"
"15260_8640.bmp"
)

kernels=(
"kernel_3.txt"
"kernel_5.txt"
"kernel_7.txt"
"kernel_9.txt"
"kernel_11.txt"
"kernel_13.txt"
)

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector")

versions_2=("sequence" "openmp")

echo "Execute experiments"
IFS=""
for exp_idx in ${!input_images[@]}; do
  for kernel in ${kernels[@]}; do
    for v1 in ${versions_1[@]}; do
      for v2 in ${versions_2[@]}; do
        echo "============================================"
        input_image=${input_images[$exp_idx]}
        kernel_name="$(basename -s .txt ${kernel})"
        output_file="${kernel_name}_${v1}_${v2}_${exp_idx}.png"
        echo "Input image: ${input_image}"
        echo "Kernel: ${kernel_name}"
        echo "Output file name: ${output_file}"
        echo "Version type 1: ${v1}"
        echo "Version type 2: ${v2}"
        echo "Working directory: ${PWD}"
        bin_file="${WORK_DIR}/${build_name_prefix}-${v2}-${v1}/bin/filter"

        if [ $v2 = "openmp" ]
        then
          export GOMP_CPU_AFFINITY="0-9"
          export OMP_NUM_THREADS=10
        fi


        if $SAVE_OUTPUT
        then
          echo "Command line: ${bin_file} ${data_dir}/${input_image} -1 -1 1 ${output_file}"
          ${bin_file} ${data_dir}/${input_image} ${data_dir}/${kernel} -1 -1 1 ${output_file}
        else
          echo "Command line: ${bin_file} ${data_dir}/${input_image} -1 -1 1"
          ${bin_file} ${data_dir}/${input_image} ${data_dir}/${kernel} -1 -1 1
        fi

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

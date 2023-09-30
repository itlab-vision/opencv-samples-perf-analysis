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

erosion_sizes=(1 2 3)

iterations_number=1

border_type=1

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector")

versions_2=("sequence" "openmp")

echo "Execute experiments"
IFS=""
for exp_idx in ${!input_images[@]}; do
  for erosion_size in ${erosion_sizes[@]}; do
    for v1 in ${versions_1[@]}; do
      for v2 in ${versions_2[@]}; do
        echo "============================================"
        input_image=${input_images[${exp_idx}]}
        output_file="${erosion_size}_${v1}_${v2}_${exp_idx}.png"
        echo "Input image: ${input_image}"
        echo "Erosion size: ${erosion_size}"
        echo "Output file name: ${output_file}"
        echo "Version type 1: ${v1}"
        echo "Version type 2: ${v2}"
        echo "Working directory: ${PWD}"
        bin_file="${WORK_DIR}/${build_name_prefix}-${v2}-${v1}/bin/erosion"

        if [ $v2 = "openmp" ]
        then
          export GOMP_CPU_AFFINITY="0-9"
          export OMP_NUM_THREADS=10
        fi


        if $SAVE_OUTPUT
        then
          echo "Command line: ${bin_file} ${data_dir}/${input_image} ${erosion_size} -1 -1 ${iterations_number} ${border_type} ${output_file}"
          ${bin_file} ${data_dir}/${input_image} ${erosion_size} -1 -1 ${iterations_number} ${border_type} ${output_file}
        else
          echo "Command line: ${bin_file} ${data_dir}/${input_image} ${erosion_size} -1 -1 ${iterations_number} ${border_type}"
          ${bin_file} ${data_dir}/${input_image} ${erosion_size} -1 -1 ${iterations_number} ${border_type}
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
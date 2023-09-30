#!/bin/bash

#SBATCH --time=3500


# Change WORK_DIR with all builds
WORK_DIR="/home/kustikova_v"
data_dir="${WORK_DIR}/data/nn/images"

# Change SAVE_OUTPUT option
SAVE_OUTPUT=false

# Change model information
model_name="alexnet"
model_weights="${WORK_DIR}/data/nn/models/${model_name}.caffemodel"
model="${WORK_DIR}/data/nn/models/${model_name}.prototxt"


batch_sizes=(1 2 4 8)

scale_factor=1
width=224
height=224
intensity_r=104
intensity_g=117
intensity_b=123

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector")

versions_2=("sequence" "openmp")

echo "Execute experiments"
IFS=""
for batch_size in ${batch_sizes[@]}; do
  for v1 in ${versions_1[@]}; do
     for v2 in ${versions_2[@]}; do
       echo "============================================"
       if [ $v2 = "openmp" ]
       then
         export GOMP_CPU_AFFINITY="0-9"
         export OMP_NUM_THREADS=10
       fi

       bin_file="${WORK_DIR}/${build_name_prefix}-${v2}-${v1}/bin/nn"
       output_file="${model_name}_${batch_size}_${v1}_${v2}.yml"

       echo "Model name: ${model_name}"
       echo "Batch size: ${batch_size}"
       echo "Output file name: ${output_file}"
       echo "Version type 1: ${v1}"
       echo "Version type 2: ${v2}"

       if $SAVE_OUTPUT
       then
         echo "Command line: ${bin_file} ${model} ${model_weights} ${data_dir} ${batch_size} ${scale_factor} ${width} ${height} ${intensity_r} ${intensity_g} ${intensity_b} ${output_file}"
         ${bin_file} ${model} ${model_weights} ${data_dir} ${batch_size} ${scale_factor} ${width} ${height} ${intensity_r} ${intensity_g} ${intensity_b} ${output_file}
       else
         echo "Command line: ${bin_file} ${model} ${model_weights} ${data_dir} ${batch_size} ${scale_factor} ${width} ${height} ${intensity_r} ${intensity_g} ${intensity_b}"
         ${bin_file} ${model} ${model_weights} ${data_dir} ${batch_size} ${scale_factor} ${width} ${height} ${intensity_r} ${intensity_g} ${intensity_b}
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
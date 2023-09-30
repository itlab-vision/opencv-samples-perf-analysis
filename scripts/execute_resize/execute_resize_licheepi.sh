#!/bin/bash


# Change WORK_DIR with all builds
WORK_DIR="/home/sipeed"
data_dir="${WORK_DIR}/data/resize"

input_images=(
"1920_1080.png"
"3840_2160.bmp"
"7680_4320.bmp"
"15260_8640.bmp"
)
output_widths=(
240
480
960
1920
480
960
1920
3840
960
1920
3840
7680
)
output_heights=(
135
270
540
1080
270
540
1080
2160
540
1080
2160
4320
)

interpolation_methods=(0 1 2 3)

build_name_prefix="opencv-samples-perf-analysis-build"

versions_1=("scalar" "vector128" "vector512")

versions_2=("sequence" "openmp")

save_result="off"

echo "Execute experiments"
IFS=""
for exp_idx in ${!input_images[@]}; do
  for method in ${interpolation_methods[@]}; do
    for v1 in ${versions_1[@]}; do
      for v2 in ${versions_2[@]}; do
        echo "============================================"
        input_image=${input_images[$exp_idx]}
        output_width=${output_widths[$exp_idx]}
        output_height=${output_heights[$exp_idx]}
        if [ $save_result = "on" ]
        then
           output_file="${method}_${v1}_${v2}_${exp_idx}.png"
        fi
        echo "Input image: ${input_image}"
        echo "Output image resolution: ${output_width}x${output_height}"
        echo "Output file name: ${output_file}"
        echo "Interpolation method: ${method}"
        echo "Version type 1: ${v1}"
        echo "Version type 2: ${v2}"
        echo "Working directory: ${PWD}"
        bin_file="${WORK_DIR}/${build_name_prefix}-${v1}-${v2}/bin/resize"
             
        if [ $v2 = "openmp" ]
        then
          export OMP_NUM_THREADS=4
          export GOMP_CPU_AFFINITY="0-3"
        fi

        echo "Command line: ${bin_file} ${data_dir}/${input_image} ${output_width} ${output_height} ${method} ${output_file}"
        ${bin_file} ${data_dir}/${input_image} ${output_width} ${output_height} ${method} ${output_file}

        if [ $v2 = "openmp" ]
        then
          unset OMP_NUM_THREADS
          unset GOMP_CPU_AFFINITY
        fi

        echo "============================================"
      done
    done
  done
done

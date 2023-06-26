# Computer vision samples for performance analysis

## Introduction

This is a repo contained computer vision samples for performance
analysis on RISC-V. Samples implemented using the well-known
[OpenCV library][opencv].

## Repo structure

- `data` is a directory that contains input data for the implemented
  samples.
- `filter` is a directory that contains sample for `filter2D` function.
- `resize` is a directory that contains sample for `resize` function.
- `svm` is a directory that contains sample of image classification using
  Support Vector Machine implemented in OpenCV.
- `nn` is a directory that contains sample of deep neural network inference
  using the OpenCV library.

## How to build OpenCV on RISC-V

```bash
cd /home/user
git clone https://github.com/opencv/opencv.git
mkdir opencv-build
cd opencv-build

TOOLCHAIN_FILE=../opencv/platforms/linux/riscv64-071-gcc.toolchain.cmake

cmake \
  -G "Unix Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
  -DWITH_OPENCL=OFF \
  -DWITH_IPP=OFF \
  -DBUILD_JAVA=OFF \
  -DBUILD_TESTS=OFF \
  -DBUILD_EXAMPLES=OFF \
  -DBUILD_SHARED_LIBS=OFF \
  -DBUILD_ZLIB=ON \
  -DBUILD_PNG=ON \
  -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
  -DCMAKE_INSTALL_PREFIX=/home/user/opencv_install \
  -DCMAKE_TOOLCHAIN_FILE=$TOOLCHAIN_FILE \
  ../opencv

make -j
make install
```

## How to build samples

```bash
mkdir perf-samples
cd perf-samples
git clone https://github.com/itlab-vision/opencv-samples-perf-analysis
mkdir opencv-samples-perf-analysis-build
cd opencv-samples-perf-analysis-build
cmake -G "Unix Makefiles" \
      -DCMAKE_BUILD_TYPE=Release \
      -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
      -DCMAKE_TOOLCHAIN_FILE=../opencv-samples-perf-analysis/riscv64-071-gcc.toolchain.cmake \
      -DOpenCV_DIR=/home/user/opencv-build \
      ../opencv-samples-perf-analysis
make -j
cd /bin
```

## How to run

```bash
./filter ../../opencv-samples-perf-analysis/data/filter/forest.jpg \ 
         ../../opencv-samples-perf-analysis/data/filter/kernel.txt \
         0 0 forest_filtered.jpg
```

<!-- LINKS -->
[opencv]: https://opencv.org

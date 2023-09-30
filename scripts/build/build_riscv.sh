#!/bin/bash


GCC_PATH=/home/user/riscv64-linux-x86_64
export PATH=$GCC_PATH/bin:${PATH}

echo "Clone OpenCV repository"
git clone https://github.com/ValentinV95/opencv
cd opencv
git remote add upstream https://github.com/opencv/opencv
git remote update
git merge upstream/4.x
cd ..

echo "Clone samples repository"
git clone https://git.itmm.unn.ru/itlab-vision/opencv-samples-perf-analysis

WORK_DIR=$PWD

function build_version() {
  OPENCV_BUILD_DIR=$1
  OPENCV_INSTALL_DIR=$2
  SAMPLES_BUILD_DIR=$3
  WITH_OPENMP=$4
  OPENCV_TOOLCHAIN_FILE=$5
  SAMPLES_TOOLCHAIN_FILE=$6

  echo "OpenCV build directory:   $WORK_DIR/${OPENCV_BUILD_DIR}"
  echo "OpenCV install directory: $WORK_DIR/${OPENCV_INSTALL_DIR}"
  echo "OpenCV toolchain file:    $WORK_DIR/opencv/platforms/linux/${OPENCV_TOOLCHAIN_FILE}"
  echo "Samples build directory:  $WORK_DIR/${SAMPLES_BUILD_DIR}"
  echo "Samples toolchain file:   $WORK_DIR/opencv-samples-perf-analysis/${SAMPLES_TOOLCHAIN_FILE}"
  echo "OpenMP support:           ${WITH_OPENMP}"

  mkdir $OPENCV_BUILD_DIR
  mkdir $OPENCV_INSTALL_DIR
  mkdir $SAMPLES_BUILD_DIR

  cd $WORK_DIR/$OPENCV_BUILD_DIR
  echo "---------------------------------------"
  echo "Build OpenCV"
  echo "Working directory:        ${PWD}"
  cmake \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
    -DWITH_OPENCL=OFF \
    -DWITH_IPP=OFF \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DWITH_TBB=OFF \
    -DWITH_PTHREADS_PF=OFF \
    -DWITH_VA=OFF \
    -DWITH_VA_INTEL=OFF \
    -DBUILD_JAVA=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_ZLIB=ON \
    -DBUILD_PNG=ON \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$WORK_DIR/$OPENCV_INSTALL_DIR \
    -DCMAKE_TOOLCHAIN_FILE=$WORK_DIR/opencv/platforms/linux/$OPENCV_TOOLCHAIN_FILE \
    $WORK_DIR/opencv
  make -j
  make install

  cd $WORK_DIR/$SAMPLES_BUILD_DIR
  echo "---------------------------------------"
  echo "Build samples"
  echo "Working directory:        ${PWD}"
  cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=riscv64 \
    -DCMAKE_TOOLCHAIN_FILE=$WORK_DIR/opencv-samples-perf-analysis/$SAMPLES_TOOLCHAIN_FILE \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DOpenCV_DIR=$WORK_DIR/$OPENCV_INSTALL_DIR/lib/cmake/opencv4 \
    $WORK_DIR/opencv-samples-perf-analysis
  make -j

  cd $WORK_DIR

  echo "Working directory:        ${PWD}"
  echo "---------------------------------------"
}



echo "======================================="
echo "Build scalar sequence version"
echo "---------------------------------------"
build_version opencv-build-scalar-sequence opencv-install-scalar-sequence \
              opencv-samples-perf-analysis-build-scalar-sequence OFF \
              riscv64-gcc.toolchain.cmake riscv64-gcc.toolchain.cmake
echo "======================================="

echo "======================================="
echo "Build scalar parallel version"
echo "---------------------------------------"
build_version opencv-build-scalar-openmp opencv-install-scalar-openmp \
              opencv-samples-perf-analysis-build-scalar-openmp ON \
              riscv64-gcc.toolchain.cmake riscv64-gcc.toolchain.cmake
echo "======================================="

echo "======================================="
echo "Build vector (512-bit) sequence version"
echo "---------------------------------------"
build_version opencv-build-vector512-sequence opencv-install-vector512-sequence \
              opencv-samples-perf-analysis-build-vector512-sequence OFF \
              riscv64-071-gcc.toolchain.cmake riscv64-071-gcc.toolchain.cmake
echo "======================================="

echo "======================================="
echo "Build vector (512-bit) parallel version"
echo "---------------------------------------"
build_version opencv-build-vector512-openmp opencv-install-vector512-openmp \
              opencv-samples-perf-analysis-build-vector512-openmp ON \
              riscv64-071-gcc.toolchain.cmake riscv64-071-gcc.toolchain.cmake
echo "======================================="

echo "======================================="
echo "Build vector (128-bit) sequence version"
echo "---------------------------------------"
HEADER_FILE=$WORK_DIR/opencv/modules/core/include/opencv2/core/hal/intrin_rvv071.hpp
sed -i "s/#define CV_SIMD512 1/#define CV_SIMD512 0/" $HEADER_FILE
build_version opencv-build-vector128-sequence opencv-install-vector128-sequence \
              opencv-samples-perf-analysis-build-vector128-sequence OFF \
              riscv64-071-gcc.toolchain.cmake riscv64-071-gcc.toolchain.cmake
sed -i "s/#define CV_SIMD512 0/#define CV_SIMD512 1/" $HEADER_FILE
echo "======================================="

echo "======================================="
echo "Build vector (128-bit) parallel version"
echo "---------------------------------------"
HEADER_FILE=$WORK_DIR/opencv/modules/core/include/opencv2/core/hal/intrin_rvv071.hpp
sed -i "s/#define CV_SIMD512 1/#define CV_SIMD512 0/" $HEADER_FILE
build_version opencv-build-vector128-openmp opencv-install-vector128-openmp \
              opencv-samples-perf-analysis-build-vector128-openmp ON \
              riscv64-071-gcc.toolchain.cmake riscv64-071-gcc.toolchain.cmake
sed -i "s/#define CV_SIMD512 0/#define CV_SIMD512 1/" $HEADER_FILE
echo "======================================="

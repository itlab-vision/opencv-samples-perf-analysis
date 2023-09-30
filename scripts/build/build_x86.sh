#!/bin/bash


source /common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0

echo "Create virtual environment"
conda create --prefix ~/opencv_perf_samples_env
echo "Activate virtual environment"
conda activate ~/opencv_perf_samples_env
echo "Install CMake in the virtual environment"
conda install cmake

echo "Clone OpenCV repository"
git clone https://github.com/ValentinV95/opencv
cd opencv
git remote add upstream https://github.com/opencv/opencv
git remote update
git merge upstream/4.x
cd ../

echo "Clone samples repository"
git clone https://git.itmm.unn.ru/itlab-vision/opencv-samples-perf-analysis.git

WORK_DIR=$PWD

function build_versions() {
  OPENCV_BUILD_DIR_PREFIX=$1
  OPENCV_INSTALL_DIR_PREFIX=$2
  SAMPLES_BUILD_DIR_PREFIX=$3
  WITH_OPENMP=$4

  echo "OpenCV build directory prefix:   ${WORK_DIR}/${OPENCV_BUILD_DIR_PREFIX}"
  echo "OpenCV install directory prefix: ${WORK_DIR}/${OPENCV_INSTALL_DIR_PREFIX}"
  echo "Samples build directory prefix:  ${WORK_DIR}/${SAMPLES_BUILD_DIR_PREFIX}"
  echo "OpenMP support:                  ${WITH_OPENMP}"

  echo "---------------------------------------"
  echo "Build scalar version of OpenCV"
  OPENCV_BUILD_DIR="${WORK_DIR}/${OPENCV_BUILD_DIR_PREFIX}-scalar"
  OPENCV_INSTALL_DIR="${WORK_DIR}/${OPENCV_INSTALL_DIR_PREFIX}-scalar"
  SAMPLES_BUILD_DIR="${WORK_DIR}/${SAMPLES_BUILD_DIR_PREFIX}-scalar"
  mkdir $OPENCV_BUILD_DIR
  mkdir $OPENCV_INSTALL_DIR
  mkdir $SAMPLES_BUILD_DIR

  cd $OPENCV_BUILD_DIR

  echo "Working directory:        ${PWD}"
  cmake -G "Unix Makefiles" \
    -DCMAKE_CXX_FLAGS="-fno-tree-vectorize -march=x86-64" \
    -DCMAKE_C_FLAGS="-fno-tree-vectorize -march=x86-64" \
    -DCV_DISABLE_OPTIMIZATION=ON \
    -DCV_ENABLE_INTRINSICS=OFF \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=x86_64 \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DWITH_TBB=OFF \
    -DWITH_PTHREADS_PF=OFF \
    -DWITH_VA=OFF \
    -DWITH_INTEL_VA=OFF \
    -DWITH_OPENCL=OFF \
    -DWITH_IPP=OFF \
    -DBUILD_JAVA=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_ZLIB=ON \
    -DBUILD_PNG=ON \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_DIR \
    $WORK_DIR/opencv
  make -j
  make install

  cd $SAMPLES_BUILD_DIR

  echo "---------------------------------------"
  echo "Build scalar version of samples"
  echo "Working directory:        ${PWD}"
  CC=gcc CXX=c++ \
  cmake -G "Unix Makefiles" \
    -DCMAKE_CXX_FLAGS="-fno-tree-vectorize -march=x86-64" \
    -DCMAKE_C_FLAGS="-fno-tree-vectorize -march=x86-64" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=x86_64 \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DOpenCV_DIR=$OPENCV_INSTALL_DIR/lib64/cmake/opencv4 \
    $WORK_DIR/opencv-samples-perf-analysis
  make -j
  echo "---------------------------------------"

  cd $WORK_DIR

  echo "---------------------------------------"
  echo "Build vector version of OpenCV"
  OPENCV_BUILD_DIR="${WORK_DIR}/${OPENCV_BUILD_DIR_PREFIX}-vector"
  OPENCV_INSTALL_DIR="${WORK_DIR}/${OPENCV_INSTALL_DIR_PREFIX}-vector"
  SAMPLES_BUILD_DIR="${WORK_DIR}/${SAMPLES_BUILD_DIR_PREFIX}-vector"
  mkdir $OPENCV_BUILD_DIR
  mkdir $OPENCV_INSTALL_DIR
  mkdir $SAMPLES_BUILD_DIR

  cd $OPENCV_BUILD_DIR

  echo "Working directory:        ${PWD}"
  cmake -G "Unix Makefiles" \
    -DCV_ENABLE_INTRINSICS=ON \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=x86_64 \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DWITH_TBB=OFF \
    -DWITH_PTHREADS_PF=OFF \
    -DWITH_VA=OFF \
    -DWITH_INTEL_VA=OFF \
    -DWITH_OPENCL=OFF \
    -DWITH_IPP=OFF \
    -DBUILD_JAVA=OFF \
    -DBUILD_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_SHARED_LIBS=OFF \
    -DBUILD_ZLIB=ON \
    -DBUILD_PNG=ON \
    -DCMAKE_BUILD_WITH_INSTALL_RPATH=ON \
    -DCMAKE_INSTALL_PREFIX=$OPENCV_INSTALL_DIR \
    $WORK_DIR/opencv
  make -j
  make install

  cd $SAMPLES_BUILD_DIR

  echo "---------------------------------------"
  echo "Build vector version of samples"
  echo "Working directory:        ${PWD}"
  CC=gcc CXX=c++ \
  cmake -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_SYSTEM_PROCESSOR=x86_64 \
    -DWITH_OPENMP=$WITH_OPENMP \
    -DOpenCV_DIR=$OPENCV_INSTALL_DIR/lib64/cmake/opencv4 \
    $WORK_DIR/opencv-samples-perf-analysis
  make -j
  echo "---------------------------------------"

  cd $WORK_DIR
}


echo "======================================="
echo "Build sequence versions"
echo "---------------------------------------"
build_versions opencv-build-sequence opencv-install-sequence \
               opencv-samples-perf-analysis-build-sequence OFF
echo "======================================="

echo "======================================="
echo "Build parallel versions"
echo "---------------------------------------"
build_versions opencv-build-openmp opencv-install-openmp \
               opencv-samples-perf-analysis-build-openmp ON
echo "======================================="

echo "Deactivate virtual environment"
conda deactivate

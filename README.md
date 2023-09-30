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
- `bow_svm` is a directory that contains sample of image classification using
  bag-of-words approach and Support Vector Machine implemented in OpenCV.
- `nn` is a directory that contains sample of deep neural network inference
  using the OpenCV library.
- `reader` is a directory that contains implementation of data readers
  (currently, Cifar-10 data reader is available).
- `utils` is a directory that contains utilities to check correctness
  of the developed applications.
- `scripts` is a directory that contains auxiliary scripts to build
  and run samples on x86 and RISC-V.

## How to build OpenCV for RISC-V

We build OpenCV on Ubuntu (for example, installed on Windows).

The GCC compiler is available [here][compiler]
(riscv64-linux-x86_64-20210618.tar.gz). It is required to extract
the archive and update environment variable `PATH`.

```bash
tar -xzf riscv64-linux-x86_64-20210618.tar.gz 
GCC_PATH=/home/user/riscv64-linux-x86_64
export PATH=$GCC_PATH/bin:${PATH}
riscv64-unknown-linux-gnu-gcc --version
```

To build OpenCV for executing on RISC-V, please, follow instructions below.

```bash
cd /home/user
git clone https://github.com/opencv/opencv.git
mkdir opencv_build
cd opencv_build

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

**Notes**:

1. Please, use cmake options `WITH_OPENMP`, `WITH_TBB`
   or `WITH_PTHREADS_PF` to build parallel versions.
1. Build scripts are located in the directory `scripts\build`.

## How to build samples for RISC-V

We build samples on the same computer where OpenCV was build.

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
      -DOpenCV_DIR=/home/user/opencv_install \
      ../opencv-samples-perf-analysis
make -j
cd /bin
```

**Notes**:

1. Please, use cmake option `WITH_OPENMP` to build parallel version.
1. build scripts are located in the directory `scripts\build`.

## How to run on RISC-V

### Filtering (filter)

```bash
./filter ../../opencv-samples-perf-analysis/data/filter/forest.jpg \ 
         ../../opencv-samples-perf-analysis/data/filter/kernel.txt \
         -1 -1 0 forest_filtered.jpg
```

**Notes:**

1. The anchor value (-1, -1) corresponds to the kernel center.
   Example for the kernel size 3x3 (Ox is a horizontal axis,
   Oy is a vertical axis):

   | x, y | x, y | x, y |
   |------|------|------|
   | 0, 0 | 0, 1 | 0, 2 |
   | 1, 0 | 1, 1 | 1, 2 |
   | 2, 0 | 2, 1 | 2, 2 |

1. The border type `0` corresponds to the constant border.

### Resizing (resize)

```bash
./resize ../../opencv-samples-perf-analysis/data/resize/forest.jpg \
         100 100 0 forest_resized.jpg
```

**Notes:**

1. `100x100` corresponds to the new size.
1. `0` corresponds nearest neighbor interpolation.


### Erosion (erosion)

```bash
./erosion ../../opencv-samples-perf-analysis/data/filter/forest.jpg \ 
        1 -1 -1  1 0 forest_erosion.jpg
```

**Notes:**

1. The anchor value (-1, -1) corresponds to the kernel center.
   Example for the kernel size 3x3. erosion_size = 1 (Ox is a horizontal axis,
   Oy is a vertical axis):

   | x, y | x, y | x, y |
   |------|------|------|
   | 0, 0 | 0, 1 | 0, 2 |
   | 1, 0 | 1, 1 | 1, 2 |
   | 2, 0 | 2, 1 | 2, 2 |

1. The border type `0` corresponds to the constant border.


### Bag-of-words with Support Vector Machine (bow_svm)

#### Train

```bash
./bow_svm train cifar-10-batches-bin/ cifar10 svm.xml \
          100 2 3 0.01 100 vocabulary.yml 32 SIFT SIFT
```

#### Test (inference)

```bash
./bow_svm inference cifar-10-batches-bin/ cifar10 \
          ../../opencv-samples-perf-analysis/data/bow_svm/svm.xml \
          ../../opencv-samples-perf-analysis/data/bow_svm/vocabulary.yml \
          SIFT SIFT output.yml
```

**Notes:**

1. Cifar10 can be downloaded [here][cifar-10].

### Neural network inference (nn)

#### AlexNet

```bash
./nn alexnet_deploy.prototxt bvlc_alexnet.caffemodel \
     ../../opencv-samples-perf-analysis/data/nn/ \
     256 1 224 224 104 117 123 output.yml
```

**Notes:**

1. AlexNet can be downloaded here ([alexnet.prototxt][alexnet-prototxt], [alexnet])
1. Output looks as follows:

```
image: ../../opencv-samples-perf-analysis/data/nn/ILSVRC2012_val_00000023.JPEG
probability: [0.99829763, 0.00056126592, 0.0003486975, 6.7908339e-05, 6.555381e-05]
class: 948
image: ../../opencv-samples-perf-analysis/data/nn/ILSVRC2012_val_00000247.JPEG
probability: [0.99999547, 4.3205505e-06, 2.8146468e-07, 2.0098549e-08, 1.2273974e-08]
class: 13
image: ../../opencv-samples-perf-analysis/data/nn/ILSVRC2012_val_00018592.JPEG
probability: [0.89906287, 0.027598409, 0.020923723, 0.0196473, 0.0062733875]
class: 625
```

#### GoogleNet

```bash
./nn googlenet.prototxt bvlc_googlenet.caffemodel \
     ../../opencv-samples-perf-analysis/data/nn/ \
     256 1 224 224 104 117 123 output.yml
```

**Notes:**

1. GoogleNet can be downloaded here ([googlenet.prototxt][googlenet-prototxt], [googlenet]).
1. Output looks as follows: 

```
image: ../../opencv-samples-perf-analysis/data/nn/LSVRC2012_val_00000023.JPEG
probability: [0.99829763, 0.00056126592, 0.0003486975, 6.7908339e-05, 6.555381e-05]
class: 948
image: ../../opencv-samples-perf-analysis/data/nn/ILSVRC2012_val_00000247.JPEG
probability: [0.99999547, 4.3205505e-06, 2.8146468e-07, 2.0098549e-08, 1.2273974e-08]
class: 13
image: ../../opencv-samples-perf-analysis/data/nn/ILSVRC2012_val_00018592.JPEG
probability: [0.89906287, 0.027598409, 0.020923723, 0.0196473, 0.0062733875]
class: 625
```

### Utilities (utils)

#### Check difference between two images (required for filtering and resizing)

```bash
./diff_images ../../opencv-samples-perf-analysis/data/filter/forest_filtered_x86.jpg \
              forest_filtered.jpg
```

#### Compute accuracy for the SVM classifier

```bash
./check_svm_accuracy cifar10 output.yml cifar-10-batches-bin/
```

#### Compute top-1 and top-5 accuracy for the NN classifier

```bash
./check_nn_accuracy output.yml \
                    ../../opencv-samples-perf-analysis/data/nn/imagenet_labels_for_caffe_models.txt
```

## Experiments

Experimental results are available [here][results-table].


<!-- LINKS -->
[opencv]: https://opencv.org
[compiler]: https://disk.yandex.ru/d/64TVJ6xxZcIMkA
[alexnet-prototxt]: https://raw.githubusercontent.com/BVLC/caffe/88c96189bcbf3853b93e2b65c7b5e4948f9d5f67/models/bvlc_alexnet/deploy.prototxt
[alexnet]: https://storage.openvinotoolkit.org/repositories/open_model_zoo/public/2022.1/alexnet/bvlc_alexnet.caffemodel
[googlenet-prototxt]: https://raw.githubusercontent.com/BVLC/caffe/88c96189bcbf3853b93e2b65c7b5e4948f9d5f67/models/bvlc_googlenet/deploy.prototxt
[googlenet]: https://storage.openvinotoolkit.org/repositories/open_model_zoo/public/2022.1/googlenet-v1/bvlc_googlenet.caffemodel
[cifar-10]: https://www.cs.toronto.edu/~kriz/cifar-10-binary.tar.gz
[results-table]: https://docs.google.com/spreadsheets/d/13w7-znPbu2YrrPI1-dvAuvQNzdqk54yIGVGt244uM8I/edit?usp=sharing

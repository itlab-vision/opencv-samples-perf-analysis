# How to build

Build scripts are located in the directory `build`.

## Build on x86

```bash
source build_x86.sh
```

## Build on RISC-V

```bash
source build_riscv.sh
```

# How to execute

Execution scripts are located in directories with prefix `execute-`.

## Execute on x86

We executed experiments on te cluster node with cluster management
system Slurm that is why we provide sbatch-scripts.

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="resize" --nodes=1 execute_resize_x86.sh
```

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="filter2d" --nodes=1 execute_filter2d_x86.sh
```

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="bowsvm_train" --nodes=1 execute_bowsvm_train_x86.sh
```

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="bowsvm_inference" --nodes=1 execute_bowsvm_inference_x86.sh
```

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="nn_inference" --nodes=1 execute_nn_inference_x86.sh
```

```bash
/common/software/intel/oneapi/setvars.sh intel64
module load gcc-9.5.0
sbatch --job-name="erosion" --nodes=1 execute_erosion_x86.sh
```

## Execute on RISC-V

### Mango Pi

```bash
./execute_resize_mangopi.sh
```

```bash
./execute_filter2d_mangopi.sh
```

```bash
./execute_bowsvm_inference_mangopi.sh
```

```bash
./execute_nn_inference_mangopi.sh
```

```bash
./execute_erosion_mangopi.sh
```

### Lichee Pi

```bash
./execute_resize_licheepi.sh
```

```bash
./execute_filter2d_licheepi.sh
```

```bash
./execute_bowsvm_inference_licheepi.sh
```

```bash
./execute_nn_inference_licheepi.sh
```

```bash
./execute_erosion_licheepi.sh
```

# How to check correctness

We suppose that OpenCV works correclty on x86 and provides target results.

### Make table with diffs for erode operation between reference images and target images

```bash
python execute_diff_erosion.py -rd ./reference_images -cd ./target images -dp ./diff_images -t licheepi
```
- `-rd` is a directory with reference images.
- `-cd` is a directory with target images.
- `-dp` is a path to the `diff_images` executable file.
- `-t` is a type of device `{x86, mangopi, licheepi}`.


### Make table with diff for filter2d operation between reference images and target images

```bash
python execute_diff_filter2d.py -rd ./reference_images -cd ./target images -dp ./diff_images -t licheepi
```

- `-rd` is a directory with reference images.
- `-cd` is a directory with target images.
- `-dp` is a path to the `diff_images` executable file.
- `-t` is a type of device `{x86, mangopi, licheepi}`.

### Make table with diff for resize operation between reference images and target images

```bash
python execute_diff_resize.py -rd ./reference_images -cd ./target images -dp ./diff_images -t licheepi
```

- `-rd` is a directory with reference images.
- `-cd` is a directory with target images.
- `-dp` is a path to the `diff_images` executable file.
- `-t` is a type of device `{x86, mangopi, licheepi}`.

### Make table with accuracy of svm execution

```bash
python3 execute_accuracy_svm.py \
-i ./licheepi_results/ -ap ./check_svm_accuracy -cd ./cifar-10-batches-bin/ -t licheepi
```

- `-i` is a directory with bowsvm inference output files.
- `-ap` is a path to the `check_svm_accuracy` executable file.
- `-cd` is a path to the Cifar10 dataset.
- `-t` is a type of device `{x86, mangopi, licheepi}`.

# How to execute series of experiments

One-time startup algorithm may produce unrepresentative times.
To obtain reliable time:

1. Copy the multi_run.pl file to the current path.
1. In the startup script, please, replace the following line:

```bash
${bin_file} ...
```

by

```bash
perl multi_run.pl ${bin_file} ...
```

# How to parse logs

```bash
python select_data_<type>.py -ifn <log file> -ofn <output filename for .xlsx>
```

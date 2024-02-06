# VNEC

## Computing Environment
### Hardware
Intel(R) Xeon(R) Gold 6240 CPU @ 2.60GHz
Kunpeng 920  CPU @ 2.60GHz
### Software
cmake (3.22.1-1ubuntu1.22.04.1)
googletest

## Build

```bash
mkdir build
cd build
cmake ..
make -j
```
## Dataset Preparation

```bash
cd data
# fetch the .tar.gz packages and decompress them to .mtx files.
make all
# convert .mtx files to .sg and .wsg files.
make convert
# create graph data links for each test group
make link
```
.sg and .wsg format graph can be loaded at a much higher speed than .mtx format graph, which needs more pre-processing.

Note that, for some network reasons, some sparse matrix data cannot be downloaded. In this case, you need to download the data manually from http://sparse.tamu.edu/ and put the mat.tar.gz in the VNEC/data/graph/mat folder. And then re-execute the command

```bash
make convert
```

## Run test demos in fp64

```bash
cd build
./bin/OpenSpB_test_operation --gtest_filter=mxv_test_VNEC_FP64.amazon*
```
Note that the test cases (sparse matrices) are written in the google test specification at the end of src/operation/test/test_spmv_vnec.cpp.
## Compile and run
```bash
./run_vnec.sh
```

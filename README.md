regression
==========

Clone this repo inside Galois app folder.

Steps to execute program on Stampede

In the parent dir, in the **CMakeLists.txt**
Remove everything and add:


```
add_subdirectory(regression)
```

**Set env variables and load modules**


```
module load cmake 
export BOOST_ROOT=$TACC_BOOST_DIR
cmake -DCMAKE_CXX_COMPILER=icpc -DCMAKE_C_COMPILER=icc ../../ 
```

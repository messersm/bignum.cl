# bignum.cl
> A fast fixed sized big number library written for C and OpenCL C.

Hi. There's not much here yet, but maybe soon there will be a fixed sized big number library for C and OpenCL C here.

## Introduction
bignum.cl provides an easy-to-use interface for mathematical operations on pre-allocated fixed sized arrays of
unsigned integers which is compatible with OpenCL C.

OpenCL C doesn't allow dynamic memory allocation, so if we want to do big number calculations using OpenCL C we need
to operate on fixed sized data. These numbers may still be arbitrary large, so there must be support for variable
sized data. Also, if a number can be 2048 bits large but only uses the lower 32 bits, operations should run respectively fast.

The bignum.cl library achieves this, by accessing pre-allocated fixed sized arrays of unsigned integers, which can be
arbitrary large. It also keeps track of the actual length of the represented numbers in order to speed up operations.

## Features
 * Basic mathematical operations on arbitrary sized numbers
 * Fast operations (hopefully)
 * Interface to the GNU MP, in order to provide more functionality
 * **No** support for negative numbers.

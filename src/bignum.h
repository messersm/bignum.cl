/**
 * @file
 * @brief Declares basic types and core functions of the bignum.cl library.
 *
 * # Memory association
 *
 * A core principle of the bignum.cl library is memory association.
 *
 *
 */
#ifndef __BIGNUM_H
#define __BIGNUM_H

/* OpenCL C defines size_t by itself, but cannot include <stdlib.h>
 * OpenCL C defines __OPENCL_VERSION__ according to the
 * OpenCL C 2.0 Specification (p. 48)
**/
#ifndef __OPENCL_VERSION__
#include <stdlib.h>
#endif

#ifndef BIGNUM_ELEM_TYPE
/**
 * @brief Defines the datatype used by bignum_elem_t
 *
 * You can change the default value by defining BIGNUM_TYPE yourself.
 * The makros BIGNUM_512, etc. will be working regardless of the
 * underlying bignum_elem_t datatype:
 *
 * With 32bit unsigned ints this code will set BIGNUM_512 to 16.
 * @code{.c}
 * #define BIGNUM_ELEM_TYPE unsigned int
 * #include "bignum.h"
 * @endcode
 *
 * With 8 bit unsigned chars this code will set BIGNUM_512 to 64.
 * @code{.c}
 * #define BIGNUM_ELEM_TYPE unsigned char
 * #include "bignum.h"
 * @endcode
 *
 * In both cases the maximum value represented by a bignum_t used
 * with BIGNUM_512 will be 2^512 - 1.
 *
 * @warning Setting BIGNUM_ELEM_TYPE to a signed datatype will result in
 *          undefined behavoir.
 */
#define BIGNUM_ELEM_TYPE unsigned long
#endif

#define BIGNUM_ELEM_SIZE sizeof(BIGNUM_ELEM_TYPE)
#define BIGNUM_ELEM_MAX ((BIGNUM_ELEM_TYPE) 0 - 1)

/** @brief The type of the elements of a big number. */
typedef BIGNUM_ELEM_TYPE bignum_elem_t;

/**
 * @brief The type of a big number.
 *
 * More description
 * @Warning None of the members of bignum_t should be changed by the user.
 *          Doing so can result in undefined behavoir and segmentation faults.
 */
typedef struct bignum {
    /** The number of elements this bignum actually uses. */
    size_t length;
    /** The number of elements this bignum can hold. */
    size_t max_length;
    /** A pointer to the actual values of this bignum. */
    bignum_elem_t *v;
} bignum_t;

/** @brief array size required for 512 bit numbers */
#define BIGNUM_512  (64 / sizeof(bignum_elem_t))
/** @brief array size required for 1024 bit numbers */
#define BIGNUM_1024 (BIGNUM_512*2)
/** @brief array size required for 2048 bit numbers */
#define BIGNUM_2048 (BIGNUM_512*4)
/** @brief array size required for 4096 bit numbers */
#define BIGNUM_4096 (BIGNUM_512*8)

/* TODO? BIGNUM_ELEM_LO and BIGNUM_ELEM_HI would be better
 * defined as a extern const bignum_elem_t but OpenCL has a problem with that:
 * Compare git commit 9dfb9e4efcd841c0ce3b8ad28b7c9744e51e967d
**/

/** @brief Bitmask for the lower half of a bignum_elem_t */
#define BIGNUM_ELEM_LO (BIGNUM_ELEM_MAX >> BIGNUM_ELEM_SIZE*4)

/** @brief Bitmask for the upper half of a bignum_elem_t */
#define BIGNUM_ELEM_HI (BIGNUM_ELEM_MAX << BIGNUM_ELEM_SIZE*4)

/**
 * @brief Associate num_elements in arr with num.
 *
 * After calling bignum_assoc() the elements arr[0] to arr[num_elements-1]
 * will be treated as a big number, which can be accessed through num.
 *
 * This code will create an array big enough to hold a 1024 bit number and
 * associate the variable x with it:
 * @code{.c}
 * bignum_t x;
 * bignum_elem_t x_elements[BIGNUM_1024];
 * bignum_assoc(&x, x_elements, BIGNUM_1024);
 * @endcode
 *
 * @param num: The number with which the elements in arr will be associated
 *             with.
 * @param arr: The array holding the actual elements.
 * @param num_elements: Number of elements from arr to associate with num.
 */
void bignum_assoc(bignum_t *num, bignum_elem_t *arr, const size_t num_elements);

void bignum_assoc_at(bignum_t *num, bignum_elem_t *arr, const size_t num_elements, const size_t index);

/**
 * @brief Synchronize bignum metadata with the underlying memory.
**/
void bignum_sync(bignum_t *num);

/*
 * @brief Synchronize the underlying memory with the bignum metadata.
 *
 * All this does, is to fill the underlying array with zeros
 * at indices with num->length <= index < num->max_length.
 *
 * After calling bignum_write, all bignum_ts associated
 * with the underlying memory can call bignum_sync() and
 * will then represent the correct number.
**/
void bignum_write(bignum_t *num);

/**
 * @brief Zero out all memory associated with num.
**/
void bignum_zero(bignum_t *num);

/*
 * @brief Set rop to the value of op.
**/
int bignum_set(bignum_t *rop, const bignum_t *op);

/**
 * @brief Set rop to the value of op.
**/
int bignum_set_ui(bignum_t *rop, const bignum_elem_t op);

/**
 * @brief Return a bignum_elem_t representing to lowest field of op.
 *
 * This returns op % (BIGNUM_ELEM_MAX+1), which will be the correct value,
 * if op <= BIGNUM_ELEM_MAX.
**/
bignum_elem_t bignum_get_ui(const bignum_t *op);

/**
 * Compare two numbers of type bignum_t
 *
 * @Returns -1 if op1 < op2, 1 if op1 > op2 and 0 if both are equal.
**/
int bignum_cmp(const bignum_t *op1, const bignum_t *op2);

/**
 * Compare a bignum_t with a bignum_elem_t.
 *
 * @Returns -1 if op1 < op2, 1 if op1 > op2 and 0 if both are equal.
**/
int bignum_cmp_ui(const bignum_t *op1, const bignum_elem_t op2);

/**
 * @brief Set rop = op1 + op2.
 *
 * @Returns 1, if an overflow occured and 0 otherwise.
**/
int bignum_add(bignum_t *rop, const bignum_t *op1, const bignum_t *op2);

/**
 * @brief Set rop = op1 + op2.
 *
 * @Returns 1, if an overflow occured and 0 otherwise.
**/
int bignum_add_ui(bignum_t *rop, const bignum_t *op1, const bignum_elem_t op2);


/**
 * @brief Set rop = op1 * op2.
 *
 * @Returns 1, if an overflow occured and 0 otherwise.
**/
int bignum_mul(bignum_t *rop, bignum_t *op1, bignum_t *op2);

/**
 * @brief Sets rop = op1 / op2.
 *
 * @Returns Remainder of the operation.
**/
bignum_elem_t bignum_divmod_ui(bignum_t *rop, const bignum_t *op1, const bignum_elem_t op2);

/**
 * @brief Return op1 % op2.
**/
bignum_elem_t bignum_mod_ui(const bignum_t *op1, const bignum_elem_t op2);

#endif // __BIGNUM_H

/**
 * @file
 * @brief Specification of bignum.h
**/
#include <stdio.h>

#include "bignum.h"
#include "test.c" // Provides run_test()

// All tests return 1 for success and 0 for failure.

/**
* @brief Associating a bignum_t number with n elements of an array
*        sets the max_length metadata to n.
**/
int test_assoc_max_length() {
    bignum_t x;
    bignum_elem_t x_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 0, 0
    };
    bignum_assoc(&x, x_elements, 16);
    return x.max_length == 16;
}

/**
 * @brief Associating a bignum_t number with an array with
 *        exact n nonzero lower elements sets the length metadata to n.
**/
int test_assoc_length() {
    bignum_t x;
    bignum_elem_t x_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 0, 0
    };
    bignum_assoc(&x, x_elements, 16);
    return x.length == 14;
}

/**
 * @brief Associating a bignum_t number with 0 elements of NULL,
 *        sets the length and max_length metadata to 0 and does
 *        not cause any errors.
**/
int test_assoc_zero_null() {
    bignum_t x;
    bignum_assoc(&x, NULL, 0);
    return x.length == 0 && x.max_length == 0;
}

/**
 * @brief Associating a bignum_t number with 0 elements of an array
 *        sets the length and max_length metadata to 0 and does
 *        not cause any errors.
**/
int test_assoc_zero() {
    bignum_t x;
    bignum_elem_t x_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 0, 0
    };

    bignum_assoc(&x, x_elements, 0);
    return x.length == 0 && x.max_length == 0;
}

/**
 * @brief Associating a bignum_t with an array, does not change the
 *        underlying data. It especially doesn't zero random data from
 *        uninitialized arrays.
**/
int test_assoc_no_data_change() {
    bignum_t x;
    bignum_elem_t data[4] = {1, 2, 3, 4};
    bignum_assoc(&x, data, 4);
    return data[0] == 1 && data[1] == 2 && data[2] == 3 && data[3] == 4;
}

/**
 * @brief Calling bignum_sync sets the correct length on a bignum_t.
**/
int test_sync_length() {
    bignum_t x;
    bignum_elem_t x_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 0, 1
    };
    bignum_assoc(&x, x_elements, 16);
    x_elements[15] = 0;

    bignum_sync(&x);
    return x.length == 14;
}

/**
 * @brief Calling bignum_zero() will zero out all associated data
 *        and nothing more.
**/
int test_zero_data() {
    bignum_t x;
    bignum_elem_t data[4] = {1, 2, 3, 4};
    bignum_assoc(&x, data, 3);
    bignum_zero(&x);
    return data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 4;
}

/**
 * @brief Calling bignum_zero() will zero out all associated data
 *        and nothing more, even if the length member of the bignum_t
 *        doesn't match with the associated data.
**/
int test_zero_data_wrong_length() {
    bignum_t x;
    bignum_elem_t data[4] = {1, 2, 3, 4};
    bignum_assoc(&x, data, 3);
    x.length = 1;
    bignum_zero(&x);
    return data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 4;
}

/**
 * @brief Calling bignum_zero() will set the length of a bignum_t to zero.
**/
int test_zero_length() {
    bignum_t x;
    bignum_elem_t data[4] = {1, 2, 3, 4};
    bignum_assoc(&x, data, 4);
    bignum_zero(&x);
    return x.length == 0;
}

/**
 * @brief Two bignum_t numbers associated with identical
 *        data and of equal length are equal.
**/
int test_cmp_equal_length_and_data() {
    bignum_t a;
    bignum_elem_t a_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 432, 1
    };
    bignum_assoc(&a, a_elements, 16);

    bignum_t b;
    bignum_elem_t b_elements[16] = {
        3232, 120, 33, 0, 0, 55, 0, 4, 24, 0, 0, 978, 0, 543, 432, 1
    };
    bignum_assoc(&b, b_elements, 16);

    return bignum_cmp(&a, &b) == 0;
}

int add_length_zero_zero() {
    bignum_t a, b, c;
    bignum_assoc(&a, NULL, 0);
    bignum_assoc(&b, NULL, 0);

    bignum_elem_t c_elements[4] = {1, 2, 3, 4};
    bignum_assoc(&c, c_elements, 4);
    bignum_add(&c, &a, &b);

    return c.length == 0;
}

int main() {
    printf("Testing bignum.h specification...\n");

    run_test("assoc() correctly sets bignum_t.max_length.", test_assoc_max_length);
    run_test("assoc() correctly sets bignum_t.length.", test_assoc_length);
    run_test("assoc() works with NULL on zero-length bignums.", test_assoc_zero_null);
    run_test("assoc() works with zero-length.", test_assoc_zero);
    run_test("assoc() does not change underlying data.", test_assoc_no_data_change);

    run_test("sync() correctly sets bignum_t.length.", test_sync_length);

    run_test("zero() zeros out all associated data.", test_zero_data);
    run_test("zero() works independent of bignum_t.length.", test_zero_data_wrong_length);
    run_test("zero() sets bignum_t.length to zero.", test_zero_length);

    run_test("cmp() returns 0 on numbers with equal length and data.", test_cmp_equal_length_and_data);
    run_test("add() correctly sets rop.length, for zero length ops.", add_length_zero_zero);

    return test_status; // included by test.c
}

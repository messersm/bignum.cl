/**
 * @file
 * @brief Specification of bignum.h
**/
#include <stdio.h>

#include "bignum.h"
#include "test.c" // Provides run_test() and assert_*

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

/*
 * @brief Adding two big numbers without overflow or carry
 *        will set the correct length in the result.
**/
int test_add_no_carry() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {1, 2, 0, 0};
    bignum_elem_t b_elem[4] = {3, 4, 5, 0};
    bignum_elem_t c_elem[4] = {4, 6, 5, 0};
    bignum_elem_t x_elem[4];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 4);

    bignum_add(&x, &a, &b);
    return assert_equal_bignum(&x, &c) && assert_equal_int(x.length, 3);
}

/*
 * @brief Adding two big numbers without overflow and with carry
 *        will set the correct data in all relevant places.
**/
int test_add_carry_no_overflow() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {1, BIGNUM_ELEM_MAX, 0, 0};
    bignum_elem_t b_elem[4] = {3, 4, 0, 0};
    bignum_elem_t c_elem[4] = {4, 3, 1, 0};
    bignum_elem_t x_elem[4];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 4);

    int ret = bignum_add(&x, &a, &b);
    return assert_equal_bignum(&x, &c) &&
           assert_equal_int(ret, 0);
}

/*
 * @brief Adding two big numbers with overflow
 *        will return 1.
**/
int test_add_overflow() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {1, 2, 0, BIGNUM_ELEM_MAX};
    bignum_elem_t b_elem[4] = {3, 4, 0, 2};
    bignum_elem_t c_elem[4] = {4, 6, 0, 1};
    bignum_elem_t x_elem[4];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 4);

    int ret = bignum_add(&x, &a, &b);
    return assert_equal_bignum(&x, &c) &&
           assert_equal_int(ret, 1);
}

/**
 * @brief Adding
**/
int test_add_data_overflow_to_zero() {
    bignum_t a, b, c;
    bignum_elem_t a_elem[3] = {BIGNUM_ELEM_MAX, BIGNUM_ELEM_MAX, 0};
    bignum_elem_t b_elem[3] = {1, 0, BIGNUM_ELEM_MAX};
    bignum_elem_t c_elem[3];

    bignum_assoc(&a, a_elem, 3);
    bignum_assoc(&b, b_elem, 3);
    bignum_assoc(&c, c_elem, 3);

    bignum_add(&c, &a, &b);
    return c_elem[0] == 0 && c_elem[1] == 0 && c_elem[2] == 0;
}

int test_add_length_overflow_to_zero() {
    bignum_t a, b, c;
    bignum_elem_t a_elem[3] = {BIGNUM_ELEM_MAX, BIGNUM_ELEM_MAX, 0};
    bignum_elem_t b_elem[3] = {1, 0, BIGNUM_ELEM_MAX};
    bignum_elem_t c_elem[3];

    bignum_assoc(&a, a_elem, 3);
    bignum_assoc(&b, b_elem, 3);
    bignum_assoc(&c, c_elem, 3);

    bignum_add(&c, &a, &b);
    return c.length == 0;
}

int test_add_length_zero_zero() {
    bignum_t a, b, c;
    bignum_assoc(&a, NULL, 0);
    bignum_assoc(&b, NULL, 0);

    bignum_elem_t c_elements[4] = {1, 2, 3, 4};
    bignum_assoc(&c, c_elements, 4);
    bignum_add(&c, &a, &b);

    return c.length == 0;
}

int test_mul_no_carry() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {1, 2, 3, 4};
    bignum_elem_t b_elem[4] = {5, 6, 7, 8};
    bignum_elem_t c_elem[8] = {
        5,                      // 0
        1*6 + 5*2,              // 1
        1*7 + 2*6 + 5*3,        // 2
        1*8 + 2*7 + 3*6 + 4*5,  // 3
        2*8 + 3*7 + 4*6,        // 4
        3*8 + 4*7,              // 5
        4*8,                    // 6
        0                       // 7
    };
    bignum_elem_t x_elem[8];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 8);
    bignum_assoc(&x, x_elem, 8);

    int ret = bignum_mul(&x, &a, &b);
    return bignum_cmp(&x, &c) == 0 && ret == 0;
}

int test_mul_carry_no_overflow() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {BIGNUM_ELEM_MAX, 0, 0, 0}; // only 1s
    bignum_elem_t b_elem[4] = {8, 0, 0, 0}; // lshift by 3 bits.
    bignum_elem_t c_elem[4] = {BIGNUM_ELEM_MAX - 7, 7, 0, 0};
    bignum_elem_t x_elem[4];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 4);

    int ret = bignum_mul(&x, &a, &b);
    return assert_equal_bignum(&x, &c) &&
           assert_equal_int(ret, 0);
}

int test_mul_overflow() {
    bignum_t a, b, c, x;
    bignum_elem_t a_elem[4] = {0, 0, 0, BIGNUM_ELEM_MAX}; // only 1s
    bignum_elem_t b_elem[4] = {8, 0, 0, 0}; // lshift by 3 bits.
    bignum_elem_t c_elem[4] = {0, 0, 0, BIGNUM_ELEM_MAX - 7};
    bignum_elem_t x_elem[4];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&b, b_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 4);

    int ret = bignum_mul(&x, &a, &b);
    return assert_equal_bignum(&c, &x) &&
           assert_equal_int(ret, 1);
}

int test_divmod_ui_no_carry() {
    // c = a / b
    bignum_t a, c, x;

    bignum_elem_t a_elem[4] = {102, 2665, 4223, 82};
    bignum_elem_t b = 41;
    bignum_elem_t c_elem[4] = {2, 65, 103, 2};
    bignum_elem_t r = 20;

    bignum_elem_t x_elem[10];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 10);

    bignum_elem_t y = bignum_divmod_ui(&x, &a, b);

    return assert_equal_bignum(&x, &c) &&
           assert_equal_elem(y, r);
}

int test_divmod_ui_carry() {
    // c = a / b
    bignum_t a, c, x;

    bignum_elem_t a_elem[4] = {15, 0, 0, BIGNUM_ELEM_MAX};

    // This is base / 2, so we shift a by n/2 bits, where n is
    // the number of bits in a bignum_elem_t.
    bignum_elem_t b = BIGNUM_ELEM_LO + 1;
    bignum_elem_t c_elem[4] = {0, 0, BIGNUM_ELEM_HI, BIGNUM_ELEM_LO};
    bignum_elem_t r = 15;

    bignum_elem_t x_elem[10];

    bignum_assoc(&a, a_elem, 4);
    bignum_assoc(&c, c_elem, 4);
    bignum_assoc(&x, x_elem, 10);

    bignum_elem_t y = bignum_divmod_ui(&x, &a, b);

    return assert_equal_bignum(&x, &c) &&
           assert_equal_elem(y, r);
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

    run_test("add() correctly sets rop.length, for zero length ops.", test_add_length_zero_zero);

    run_test("add() works, if no carry occurs.", test_add_no_carry);
    run_test("add() works, if a carry, but no overflow occurs.", test_add_carry_no_overflow);
    run_test("add() works, if an overflow occurs.", test_add_overflow);

    run_test("add() sets correct data when overflowing to zero.", test_add_data_overflow_to_zero);
    run_test("add() sets correct length when overflowing to zero.", test_add_length_overflow_to_zero);

    run_test("mul() with no carry works and returns 0.", test_mul_no_carry);
    run_test("mul() with carry works and returns 0, if no overflow occurs.", test_mul_carry_no_overflow);
    run_test("mul() with carry works and returns 1, if an overflow occurs.", test_mul_overflow);

    run_test("divmod_ui() without carry works and returns the correct remainder.", test_divmod_ui_no_carry);
    run_test("divmod_ui() with carry works and returns the correct remainder.", test_divmod_ui_carry);

    return test_status; // included by test.c
}

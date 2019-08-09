#include "bignum.h"

/*
 * Memory association and handling:
 *  - bignum_assoc()
 *  - bignum_sync()
 *  - bignum_zero()
**/
void bignum_sync(bignum_t *num) {
    // Synchronize bignum metadata with the underlying memory.
    num->length = 0;
    for (int i=0; i < num->max_length; i++)
        if (num->v[i] != 0)
            num->length = i+1;
}

void bignum_assoc(bignum_t *num, bignum_elem_t *arr, const size_t num_elements){
    // Associate num_elements in arr with num.
    num->max_length = num_elements;
    num->v = arr;
    bignum_sync(num);
}

void bignum_zero(bignum_t *num) {
    // Zero out all memory associated with num.
    for (int i=0; i < num->max_length; i++)
        num->v[i] = 0;
    num->length = 0;
}

/*
 * Setting big numbers:
 *  - bignum_set()
 *  - bignum_set_ui()
**/
int bignum_set(bignum_t *rop, const bignum_t *op) {
    // Set rop to the value of op.
    // Returns 0 on success and -1 otherwise.
    if (rop->max_length < op->length)
        return -1;

    for (int i=0; i < op->length; i++)
        rop->v[i] = op->v[i];
    rop->length = op->length;
    return 0;
}

int bignum_set_ui(bignum_t *rop, const bignum_elem_t op) {
    // Set rop to the value of op.
    // Returns 0 on success and -1 otherwise.
    if (op == 0)
        rop->length = 0;
    else {
        if (rop->max_length == 0)
            return -1;
        else {
            rop->length = 1;
            rop->v[0] = op;
        }
    }
    return 0;
}

/*
 * Comparing big integers
 *  - bignum_cmp()
 *  - bignum_cmp_ui()
**/
int bignum_cmp(const bignum_t *op1, const bignum_t *op2) {
    //
    // Returns -1 if op1 < op2, 1 if op1 > op2 and 0 if both are equal.
    if (op1->length > op2->length)
        return 1;
    else if(op1->length < op2->length)
        return -1;

    for (int i = op1->length-1; i>=0; i--) {
        if (op1->v[i] < op2->v[i])
            return -1;
        else if (op1->v[i] > op2->v[i])
            return 1;
    }
    return 0;
}

int bignum_cmp_ui(const bignum_t *op1, const bignum_elem_t op2) {
    if (op1->length > 1)
        return 1;
    else if(op1->length == 0)
        if (op2 == 0)
            return 0;
        else
            return -1;
    else if(op1->v[0] > op2)
        return 1;
    else if(op1->v[0] < op2)
        return -1;
    else
        return 0;
}

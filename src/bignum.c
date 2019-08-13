#include "bignum.h"

/*
 * Memory association and handling:
 *  - bignum_assoc()
 *  - bignum_sync() -> TODO: Rename to bignum_read
 *  - bignum_zero()
 *  TODO: Add bignum_write
**/
void bignum_sync(bignum_t *num) {
    // Synchronize bignum metadata with the underlying memory.
    num->length = 0;
    for (int i=0; i < num->max_length; i++)
        if (num->v[i] != 0)
            num->length = i+1;
}

void bignum_write(bignum_t *num) {
    for (int i=num->length; i < num->max_length; i++)
        num->v[i] = 0;
}

void bignum_assoc(bignum_t *num, bignum_elem_t *arr, const size_t num_elements){
    // Associate num_elements in arr with num.
    num->max_length = num_elements;
    num->v = arr;
    bignum_sync(num);
}

void bignum_assoc_at(bignum_t *num, bignum_elem_t *arr, const size_t num_elements, const size_t index) {
    // Associate num_elements in arr with num.
    num->max_length = num_elements;
    num->v = &arr[num_elements*index];
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
 * Getting big numbers:
 *  - bignum_get_ui()
**/
bignum_elem_t bignum_get_ui(const bignum_t *op) {
    if (op->length == 0)
        return 0;
    else
        return op->v[0];
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

/*
 *  Calculating big numbers
**/
int bignum_add(bignum_t *rop, const bignum_t *op1, const bignum_t *op2) {
    // rop = op1 + op2
    // Return 1, if the operation caused an overflow and 0 otherwise.
    int carry = 0;
    bignum_elem_t result;
    const bignum_t *shorter, *longer;
    size_t length = 0;

    // Find longer operand.
    if (op1->length > op2->length) {
        shorter = op2;
        longer = op1;
    }
    else {
        shorter = op1;
        longer = op2;
    }

    // Calculate the maximum length.
    size_t max_length;
    if (longer->length + 1 > rop->max_length)
        max_length = rop->max_length;
    else
        max_length = longer->length + 1;

    // Add all numbers from the shorter and longer operand
    int i;
    for (i=0; i < shorter->length && i<max_length; i++) {
        result = carry + shorter->v[i] + longer->v[i];
        if (result < shorter->v[i]) {
            if (result > 0)
                length = i + 1;
            carry = 1;
        }
        else {
            length = i + 1;
            carry = 0;
        }
        rop->v[i] = result;
    }

    // Add all numbers from the longer operand.
    for (i=shorter->length; i < longer->length && i<max_length; i++) {
        result = carry + longer->v[i];
        if (result < longer->v[i]) {
            if (result > 0)
                length = i + 1;
            carry = 1;
        }
        else {
            length = i + 1;
            carry = 0;
        }
        rop->v[i] = result;
    }

    if (carry != 0 && max_length > longer->length) {
        rop->v[longer->length] = carry;
        length = longer->length + 1;
        carry = 0;
    }

    rop->length = length;
    return carry;
}

int bignum_add_ui(bignum_t *rop, const bignum_t *op1, const bignum_elem_t op2) {
    int carry = 0;
    bignum_elem_t result;

    // Calculate the maximum length.
    size_t max_length;
    if (op1->length + 1 > rop->max_length)
        max_length = rop->max_length;
    else
        max_length = op1->length + 1;

    size_t length = 0;
    if (rop->max_length > 0) {
        result = op1->v[0] + op2;
        if (result < op2) {
            if (result > 0)
                length = 1;
            carry = 1;
        }
        else {
            length = 1;
            carry = 0;
        }
    }
    else if (op2 > 0)
        carry = 1;

    // Add all numbers from op1 + carry.
    int i;
    for (i=1; i<max_length; i++) {
        result = carry + op1->v[i];
        if (result < op1->v[i]) {
            if (result > 0)
                length = i + 1;
            carry = 1;
        }
        else {
            length = i + 1;
            carry = 0;
        }
        rop->v[i] = result;
    }

    if (carry != 0 && max_length > op1->length) {
        rop->v[op1->length] = carry;
        length = op1->length + 1;
        carry = 0;
    }

    rop->length = length;
    return carry;
}

static inline bignum_elem_t lo(bignum_elem_t elem) {
    // Return the value of the lower bits of elem.
    return elem & BIGNUM_ELEM_LO;
}

static inline bignum_elem_t hi(bignum_elem_t elem) {
    // Return the value of the higher bits of elem.
    return (elem & BIGNUM_ELEM_HI) >> BIGNUM_ELEM_SIZE * 4;
}

int bignum_mul(bignum_t *rop, bignum_t *op1, bignum_t *op2) {
    // rop = op1 * op2
    bignum_elem_t result;
    bignum_elem_t lo1, hi1, lo2, hi2;
    bignum_elem_t carry = 0;
    bignum_elem_t elem1, elem2;

    size_t length = 0;
    int overflow = 0;
    int i, j;

    size_t max_length = op1->length + op2->length;
    if (max_length > rop->max_length)
        max_length = rop->max_length;

    // Calculation starts here.
    for (int pos=0; pos<max_length; pos++) {
        result = carry;
        carry = 0;

        for (i=0; i<op1->length; i++) {
            j = pos - i;
            if (j >= op2->length)
                continue;
            else if (j < 0)
                break;

            elem1 = op1->v[i];
            elem2 = op2->v[j];
            lo1 = lo(elem1);
            lo2 = lo(elem2);
            hi1 = hi(elem1);
            hi2 = hi(elem2);

            result += lo1 * lo2;
            result += (hi1 * lo2) << BIGNUM_ELEM_SIZE * 4;
            result += (lo1 * hi2) << BIGNUM_ELEM_SIZE * 4;

            carry += hi1 * hi2;
            carry += hi(hi1 * lo2);
            carry += hi(lo1 * hi2);
        }

        if (result != 0)
            length = pos+1;

        rop->v[pos] = result;
    }

    if (carry != 0 || length < op1->length || length < op2->length)
        overflow = 1;

    rop->length = length;
    return overflow;
}

bignum_elem_t bignum_divmod_ui(bignum_t *rop, const bignum_t *op1, const bignum_elem_t op2) {
    // rop = op1 / op2
    // Returns remainder.

    // Calculate quotient and remainder of base / op2.
    // This is used for later calculating the remainder
    bignum_elem_t base_q;
    bignum_elem_t base_r;

    // base_r == elem_max + 1 (mod op2)
    base_r = (BIGNUM_ELEM_MAX % op2 + 1) % op2;

    // For this, consider base = 16, elem_max = 15 and op2 = 8:
    // 16 = 2 * 8 + 0
    // 15 = 1 * 8 + 7
    //      q  op2  r
    // So we have to add 1 exactly if base_r is 0.
    if (base_r == 0)
        base_q = BIGNUM_ELEM_MAX / op2 + 1;
    else
        base_q = BIGNUM_ELEM_MAX / op2;

    bignum_elem_t last_r;
    bignum_elem_t remainder = 0;
    bignum_elem_t result;

    // result = v[i] / op2 + remainder * base_q

    int i;
    size_t length = 0;

    size_t max_length;
    if (op1->length > rop->max_length)
        max_length = rop->max_length;
    else
        max_length = op1->length;

    // "Skip" too large indicies, but calculate the remainder.
    for (i=op1->length-1; i>=max_length; i--) {
        last_r = remainder;
        result = op1->v[i] / op2;
        result += last_r * base_q;
        remainder = op1->v[i] % op2;
        remainder += last_r * base_r;
    }

    for (i=max_length-1; i>=0; i--) {
        last_r = remainder;
        result = op1->v[i] / op2;
        result += last_r * base_q;
        remainder = op1->v[i] % op2;
        remainder += last_r * base_r;

        rop->v[i] = result;
        if (length == 0 && result != 0)
            length = i+1;
    }

    rop->length = length;
    return remainder;
}

bignum_elem_t bignum_mod_ui(const bignum_t *op1, const bignum_elem_t op2) {
    // rop = op1 / op2
    // Returns remainder.

    // Calculate quotient and remainder of base / op2.
    // This is used for later calculating the remainder
    bignum_elem_t base_r;

    // base_r == elem_max + 1 (mod op2)
    base_r = (BIGNUM_ELEM_MAX % op2 + 1) % op2;

    bignum_elem_t last_r;
    bignum_elem_t remainder = 0;

    for (int i=op1->length-1; i>=0; i--) {
        last_r = remainder;
        remainder = op1->v[i] % op2;
        remainder += last_r * base_r;
    }

    return remainder;
}

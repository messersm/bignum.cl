#include <stdio.h>
#include "bignum.h"

int assert_equal_bignum(bignum_t *actual, bignum_t *expected) {
    int ret = bignum_cmp(expected, actual);

    if (ret != 0) {
        printf(" * assert_equal_bignum() failed:\n");
        printf(" * Actual  : ");
        for (int i=0; i<actual->length; i++)
            printf("%lu, ", actual->v[i]);
        printf("\n");
        printf(" * Expected: ");
        for (int i=0; i<expected->length; i++)
            printf("%lu, ", expected->v[i]);
        printf("\n");
    }

    return ret == 0;
}

int assert_equal_int(int actual, int expected) {
    int ret = expected == actual;
    if (ret != 1) {
        printf(" * assert_equal_int() failed:\n");
        printf(" * Actual  : %d\n", actual);
        printf(" * Expected: %d\n", expected);
    }

    return ret == 1;
}

int test_status = 0;

void run_test(char *desc, int (*test)()) {
    int ret = test();
    if (ret == 1)
        printf("[ OK ] %s\n", desc);
    else if (ret == 0){
        printf("[FAIL] %s\n", desc);
        test_status = 1;
    }
    else if(ret == -1){
        printf("[TODO] %s\n", desc);
        test_status = -1;
    }
    else {
        printf("[????] %s\n", desc);
        test_status = -2;
    }
}

/**
 * @file
 * @brief Specification of bignum.h
**/
#include <stdio.h>

// #include "bignum_tests.c"
#include "tests_info.c.tmp" // test_names, test_count, test_functions

int test_status = 0;

void eval_test_result(const char *name, int ret) {
    if (ret == 1)
        printf("[ OK ] %s\n", name);
    else if (ret == 0){
        printf("[FAIL] %s\n", name);
        test_status = 1;
    }
    else if(ret == -1){
        printf("[TODO] %s\n", name);
        test_status = -1;
    }
    else {
        printf("[????] %s\n", name);
        test_status = -2;
    }
}

void run_test(const char *name, int (*test)()) {
    eval_test_result(name, test());
}

int main() {
    printf("Testing bignum from C.\n");

    for (int i=0; i<test_count; i++) {
        run_test(test_names[i], *test_functions[i]);
    }
    return test_status; // included by helpers.c
}

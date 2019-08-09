#include <stdio.h>

int test_status = 0;

void run_test(char *desc, int (*test)()) {
    int ret = test();
    if (ret == 1)
        printf("[ OK ] %s\n", desc);
    else {
        printf("[FAIL] %s\n", desc);
        test_status = 1;
    }
}

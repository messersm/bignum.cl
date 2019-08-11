bignum.o: src/bignum.c src/bignum.h
	gcc -c -Wall -Werror -fpic src/bignum.c

c_tests: bignum.o src/bignum.c src/bignum.h tests/tests.c tests/c_tests.c
	python scripts/wrap_tests.py --info tests/tests.c > tests/tests_info.c.tmp
	gcc -L. -I src -I tests -o c_tests.out tests/c_tests.c bignum.o
	./c_tests.out

cl_tests: bignum.o src/bignum.c src/bignum.h tests/tests.c tests/cl_tests.c
	python scripts/wrap_tests.py --info tests/tests.c > tests/tests_info.c.tmp
	python scripts/wrap_tests.py tests/tests.c > tests/tests_wrappers.cl.tmp
	gcc -L. -I src -o cl_tests.out tests/cl_tests.c bignum.o -lOpenCL
	./cl_tests.out

tests: c_tests cl_tests

bignum.o: src/bignum.c src/bignum.h
	gcc -c -Wall -Werror -fpic src/bignum.c

test_bignum.out: bignum.o tests/test.c tests/test_bignum.c
	gcc -L. -I src -o test_bignum.out tests/test_bignum.c bignum.o

run_bignum_test: test_bignum.out
	./test_bignum.out

run_tests: run_bignum_test

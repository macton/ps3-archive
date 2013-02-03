half_test: float_test_vector.o half.o half_test.o
	gcc float_test_vector.o half.o half_test.o -o half_test
conversion_test: float_test_vector.o half.o conversion_test.o
	gcc float_test_vector.o half.o conversion_test.o -o conversion_test
float_test_vector.o: float_test_vector.c
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c float_test_vector.c
half.o: half.c int_insn.h
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c half.c
half_test.o: half_test.c half.h test_float.h float_test_vector.h
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c half_test.c
conversion_test.o: conversion_test.c half.h test_float.h float_test_vector.h
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c conversion_test.c

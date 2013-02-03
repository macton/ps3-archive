conversion_test: randomized_test_vector.o float_test_vector.o half_convert.o conversion_test.o
	gcc randomized_test_vector.o float_test_vector.o half_convert.o conversion_test.o -o conversion_test
conversion_test-ilm: randomized_test_vector.o float_test_vector.o ilm_half_convert.o conversion_test.o
	gcc randomized_test_vector.o float_test_vector.o ilm_half_convert.o conversion_test.o -o conversion_test-ilm
float_test_vector.o: float_test_vector.c
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c float_test_vector.c
randomized_test_vector.o: randomized_test_vector.c
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c randomized_test_vector.c
half_convert.o: half_convert.c int_insn.h
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c half_convert.c
ilm_half_convert.o: ilm_half_convert.c
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c ilm_half_convert.c
conversion_test.o: conversion_test.c test_float.h float_test_vector.h
	gcc -O3 -fstrict-aliasing -pedantic -std=c99 -Wall -c conversion_test.c

float_test: float_test_vector.o float.o float_test.o
	gcc float_test_vector.o float_test.o float.o softfloat.o -o float_test -lm
float_test_vector.o: float_test_vector.c
	gcc -O3 -fno-strict-aliasing -pedantic -std=c99 -Wall -c float_test_vector.c
float.o: float.c
	gcc -O3 -fno-strict-aliasing -pedantic -std=c99 -Wall -c float.c
float_test.o: float_test.c test_float.h float_test_vector.h
	gcc -O3 -fno-strict-aliasing -pedantic -std=c99 -Wall -c float_test.c

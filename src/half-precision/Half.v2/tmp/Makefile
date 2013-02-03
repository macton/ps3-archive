half_test: float_test_vector.o half.o half_test.o
	gcc float_test_vector.o half.o half_test.o -o half_test
float_test_vector.o: float_test_vector.c
	gcc -O3 -fno-strict-aliasing -Wall -std=c99 -Wall -c float_test_vector.c
half.o: half.c
	gcc -O3 -fno-strict-aliasing -Wall -std=c99 -Wall -c half.c
half_test.o: half_test.c
	gcc -O3 -fno-strict-aliasing -Wall -std=c99 -Wall -c half_test.c

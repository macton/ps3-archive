half.elf : half_convert.o half_test.o
	gcc half_convert.o half_test.o -o half.elf
half_convert.o : half_convert.c
	gcc -fomit-frame-pointer -funroll-loops -O3 -std=c99 -fstrict-aliasing -c half_convert.c -o half_convert.o
	gcc -fomit-frame-pointer -funroll-loops -O3 -std=c99 -fstrict-aliasing -S -c half_convert.c 
half_test.o : half_test.c
	gcc -fomit-frame-pointer -funroll-loops -O3 -std=c99 -fstrict-aliasing -c half_test.c -o half_test.o
half_test.c : half_test.bc
	export BC_LINE_LENGTH=512 && bc -q half_test.bc > half_test.c
half_test.bc : generate_half_test
	./generate_half_test > half_test.bc
generate_half_test : generate_half_test.o
	gcc generate_half_test.o  -o generate_half_test
generate_half_test.o : generate_half_test.c
	gcc -c generate_half_test.c  -o generate_half_test.o

run: half.elf
	./half.elf

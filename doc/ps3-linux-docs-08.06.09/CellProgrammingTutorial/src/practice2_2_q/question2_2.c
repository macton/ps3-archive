#include <stdio.h>

#define SIZE    (16)

unsigned short in[SIZE] = {  1,  2,  3,  4,  5,  6,  7,  8,
                             9, 10, 11, 12, 13, 14, 15, 16 };
float          out[SIZE];

int main(int argc, char **argv)
{
    int i;

    for (i = 0; i < SIZE; i++) {
        out[i] = (float) in[i] * 0.1f;
    }

    for (i = 0; i < SIZE; i++) {
        printf("out[%02d]=%0.1f\n", i, out[i]);
    }

    return 0;
}

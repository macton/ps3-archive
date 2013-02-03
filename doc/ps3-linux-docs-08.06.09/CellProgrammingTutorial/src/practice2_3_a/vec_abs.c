#include <stdio.h>
#include <altivec.h>

#define SIZE    (16)

float in[SIZE] __attribute__((aligned(16))) = {  1,  -2,  3,  -4,
                                                 5,  -6,  7,  -8,
                                                 9, -10, 11, -12,
                                                13, -14, 15, -16 };
float out[SIZE] __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    int i;

    __vector float *vin  = (__vector float *) in;
    __vector float *vout = (__vector float *) out;
    __vector float vin_negative;
    __vector unsigned int vpat;

    __vector float vzero  = (__vector float) {  0.0f,  0.0f,  0.0f,  0.0f };
    __vector float vminus = (__vector float) { -1.0f, -1.0f, -1.0f, -1.0f };

    for (i = 0; i < SIZE/4; i++) {
        vpat = vec_cmpgt(vin[i], vzero);
        vin_negative = vec_madd(vin[i], vminus, vzero);
        vout[i] = vec_sel(vin_negative, vin[i], vpat);
    }

    for (i = 0; i < SIZE; i++) {
        printf("out[%02d]=%0.0f\n", i, out[i]);
    }

    return 0;
}

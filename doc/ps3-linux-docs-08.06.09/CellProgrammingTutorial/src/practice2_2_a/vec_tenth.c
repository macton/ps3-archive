#include <stdio.h>
#include <altivec.h>

#define SIZE    (16)

unsigned short in[SIZE] __attribute__((aligned(16))) = {  1,  2,  3,  4,  5,  6,  7,  8,
                                                          9, 10, 11, 12, 13, 14, 15, 16 };
float          out[SIZE] __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    int i;

    __vector unsigned short *vin = (__vector unsigned short *) in;
    __vector float *vout = (__vector float *) out;
    __vector unsigned char vpat1 = (__vector unsigned char) { 0x10, 0x10, 0x00, 0x01,
                                                              0x10, 0x10, 0x02, 0x03,
                                                              0x10, 0x10, 0x04, 0x05,
                                                              0x10, 0x10, 0x06, 0x07 };
    __vector unsigned char vpat2 = (__vector unsigned char) { 0x10, 0x10, 0x08, 0x09,
                                                              0x10, 0x10, 0x0a, 0x0b,
                                                              0x10, 0x10, 0x0c, 0x0d,
                                                              0x10, 0x10, 0x0e, 0x0f };
    __vector unsigned int vtmp;
    __vector float vftmp;
    __vector unsigned short vzero = (__vector unsigned short) { 0, 0, 0, 0, 0, 0, 0, 0 };
    __vector float vfzero = (__vector float) { 0.0f, 0.0f, 0.0f, 0.0f };
    __vector float vtenth = (__vector float) { 0.1f, 0.1f, 0.1f, 0.1f };

    for (i = 0; i < SIZE/8; i++) {
        vtmp = vec_perm(vin[i], vzero, vpat1);
        vftmp = vec_ctf(vtmp, 0);
        vout[i*2] = vec_madd(vftmp, vtenth, vfzero);

        vtmp = vec_perm(vin[i], vzero, vpat2);
        vftmp = vec_ctf(vtmp, 0);
        vout[i*2+1] = vec_madd(vftmp, vtenth, vfzero);
    }

    for (i = 0; i < SIZE; i++) {
        printf("out[%02d]=%0.1f\n", i, out[i]);
    }

    return 0;
}

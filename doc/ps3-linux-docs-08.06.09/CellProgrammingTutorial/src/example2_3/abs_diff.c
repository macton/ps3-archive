#include <stdio.h>
#include <altivec.h>

int a[16] __attribute__((aligned(16))) = {  1,  2,  3,  4,  5,  6,  7,  8,
                                            9, 10, 11, 12, 13, 14, 15, 16 };
int b[16] __attribute__((aligned(16))) = { 16, 15, 14, 13, 12, 11, 10,  9,
                                            8,  7,  6,  5,  4,  3,  2,  1 };
int c[16] __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    __vector signed int *va = (__vector signed int *) a;
    __vector signed int *vb = (__vector signed int *) b;
    __vector signed int *vc = (__vector signed int *) c;
    __vector signed int vc_true, vc_false;
    __vector unsigned int vpat;

    int i;

    for (i = 0; i < 4; i++) {
        vpat = vec_cmpgt(va[i], vb[i]);
        vc_true  = vec_sub(va[i], vb[i]);
        vc_false = vec_sub(vb[i], va[i]);
        vc[i] = vec_sel(vc_false, vc_true, vpat);
    }

    for (i = 0; i < 16; i++) {
        printf("c[%02d]=%2d\n", i, c[i]);
    }

    return 0;
}

/*
 * FILE: vec_add.c
 */
#include <stdio.h>
#include <altivec.h>

/*
 * declares input/output scalar varialbes
 */
int a[4] __attribute__((aligned(16))) = { 1, 3, 5, 7 };
int b[4] __attribute__((aligned(16))) = { 2, 4, 6, 8 };
int c[4] __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    /*
     * declares vector variables which points to scalar arrays
     */
    __vector signed int *va = (__vector signed int *) a;
    __vector signed int *vb = (__vector signed int *) b;
    __vector signed int *vc = (__vector signed int *) c;

    /*
     * adds four signed intergers at once
     */
    *vc = vec_add(*va, *vb);    // 1 + 2, 3 + 4, 5 + 6, 7 + 8

    /*
     * output results
     */
    printf("c[0]=%d, c[1]=%d, c[2]=%d, c[3]=%d\n", c[0], c[1], c[2], c[3]);

    return 0;
}

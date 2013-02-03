#include <stdio.h>
#include <altivec.h>

#define MAX_NUM (1024)

int main(int argc, char **argv)
{
    int i;
    int sum;
    int *psum;

    __vector signed int va    = (__vector signed int) { 1, 2, 3, 4 };
    __vector signed int vsum  = (__vector signed int) { 0, 0, 0, 0 };
    __vector signed int vstep = (__vector signed int) { 4, 4, 4, 4 };

    for (i = 1; i <= MAX_NUM; i += 4) {
        vsum = vec_add(vsum, va);
        va   = vec_add(va, vstep);
    }

    psum = (int *) &vsum;
    sum  = psum[0] + psum[1] + psum[2] + psum[3];

    printf("sum: %d\n", sum);

    return 0;
}

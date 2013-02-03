#include <stdio.h>
#include <altivec.h>

int a[16]  __attribute__((aligned(16))) = {  1,  2,  3,  4,
                                             5,  6,  7,  8,
                                             9, 10, 11, 12,
                                            13, 14, 15, 16 };
int aT[16] __attribute__((aligned(16)));

void print_matrix(int *matrix)
{
    int i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            printf("%2d ", matrix[i * 4 + j]);
        }
        printf("\n");
    }
    return;
}

int main(int argc, char **argv)
{
    __vector signed int *va  = (__vector signed int *) a;
    __vector signed int *vaT = (__vector signed int *) aT;
    __vector signed int vtmp[4];
    __vector unsigned char vpat1 = (__vector unsigned char) { 0x00, 0x01, 0x02, 0x03,
                                                              0x04, 0x05, 0x06, 0x07,
                                                              0x10, 0x11, 0x12, 0x13,
                                                              0x14, 0x15, 0x16, 0x17 };
    __vector unsigned char vpat2 = (__vector unsigned char) { 0x08, 0x09, 0x0a, 0x0b,
                                                              0x0c, 0x0d, 0x0e, 0x0f,
                                                              0x18, 0x19, 0x1a, 0x1b,
                                                              0x1c, 0x1d, 0x1e, 0x1f };
    __vector unsigned char vpat3 = (__vector unsigned char) { 0x00, 0x01, 0x02, 0x03,
                                                              0x10, 0x11, 0x12, 0x13,
                                                              0x08, 0x09, 0x0a, 0x0b,
                                                              0x18, 0x19, 0x1a, 0x1b };
    __vector unsigned char vpat4 = (__vector unsigned char) { 0x04, 0x05, 0x06, 0x07,
                                                              0x14, 0x15, 0x16, 0x17,
                                                              0x0c, 0x0d, 0x0e, 0x0f,
                                                              0x1c, 0x1d, 0x1e, 0x1f };

    printf("--- original matrix ---\n");
    print_matrix(a);

    /* vec_perm() part 1 */
    vtmp[0] = vec_perm(va[0], va[2], vpat1);
    vtmp[1] = vec_perm(va[1], va[3], vpat1);
    vtmp[2] = vec_perm(va[0], va[2], vpat2);
    vtmp[3] = vec_perm(va[1], va[3], vpat2);

    printf("--- transform 1 ---\n");
    print_matrix((int *) vtmp);

    /* vec_perm() part 2 */
    vaT[0] = vec_perm(vtmp[0], vtmp[1], vpat3);
    vaT[1] = vec_perm(vtmp[0], vtmp[1], vpat4);
    vaT[2] = vec_perm(vtmp[2], vtmp[3], vpat3);
    vaT[3] = vec_perm(vtmp[2], vtmp[3], vpat4);

    printf("--- transform 2 ---\n");
    print_matrix(aT);

    return 0;
}

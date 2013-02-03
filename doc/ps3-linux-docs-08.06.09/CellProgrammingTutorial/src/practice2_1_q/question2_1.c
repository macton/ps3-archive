#include <stdio.h>

#define MAX_NUM (1024)

int main(int argc, char **argv)
{
    int i;
    int sum = 0;

    for (i = 1; i <= MAX_NUM; i++) {
        sum += i;
    }

    printf("sum: %d\n", sum);

    return 0;
}

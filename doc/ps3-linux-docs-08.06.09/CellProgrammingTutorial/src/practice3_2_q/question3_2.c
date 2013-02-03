#include <stdio.h>

#define LENGTH  (48)

char in[LENGTH] = "A Quick Brown Fox Jumps Over The Lazy Dog.\0";
char out[LENGTH];

int main(int argc, char **argv)
{
    int i;
    char offset = 'a' - 'A';

    for (i = 0; i < sizeof(in); i++) {
        if (in[i] >= 'a' && in[i] <= 'z') {
            out[i] = in[i] - offset;
        } else {
            out[i] = in[i];
        }
    }

    printf("in  = %s\n", in);
    printf("out = %s\n", out);

    return 0;
}

#include <stdio.h>

void rgb2y(unsigned char *src, unsigned char *dst, int num)
{
    int i;
    float r, g, b, y;
    float rconst = 0.29891f;
    float gconst = 0.58661f;
    float bconst = 0.11448f;
    unsigned int max = 255;

    for (i = 0; i < num; i++) {
        r = (float) src[i * 4];
        g = (float) src[i * 4 + 1];
        b = (float) src[i * 4 + 2];

        y = r * rconst + g * gconst + b * bconst;

        if (y > max) {
            y = max;
        }

        dst[i * 4] = dst[i * 4 + 1] = dst[i * 4 + 2] = (unsigned char) y;
    }

    return;
}

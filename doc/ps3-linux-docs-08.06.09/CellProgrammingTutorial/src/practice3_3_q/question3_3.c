#include <stdio.h>
#include <altivec.h>

__vector float func(__vector float vx)
{
    __vector float vy;
    __vector float va = (__vector float) {   5.0f,   5.0f,   5.0f,   5.0f };
    __vector float vb = (__vector float) { -16.0f, -16.0f, -16.0f, -16.0f };
    __vector float vc = (__vector float) { -36.0f, -36.0f, -36.0f, -36.0f };
    __vector float vd = (__vector float) {  64.0f,  64.0f,  64.0f,  64.0f };
    __vector float ve = (__vector float) { 192.0f, 192.0f, 192.0f, 192.0f };

    vy = vec_madd(va, vx, vb);
    vy = vec_madd(vy, vx, vc);
    vy = vec_madd(vy, vx, vd);
    vy = vec_madd(vy, vx, ve);

    return vy;
}

float calc_integral(float start, float end, float delta)
{
    int i;
    float *sum;

    __vector float vx     = (__vector float) { start+delta*0, start+delta*1,
                                               start+delta*2, start+delta*3 };
    __vector float vsum   = (__vector float) { 0.0f, 0.0f, 0.0f, 0.0f };
    __vector float vdelta = (__vector float) { delta, delta, delta, delta };
    __vector float vstep  = (__vector float) { 4.0f, 4.0f, 4.0f, 4.0f };

    for (i = 0; i < (end-start)/delta; i += 4) {
        vsum = vec_madd(func(vx), vdelta, vsum);
        vx   = vec_madd(vdelta, vstep, vx);
    }

    sum = (float *) &vsum;

    return (sum[0] + sum[1] + sum[2] + sum[3]);
}

int main(int argc, char **argv)
{
    float start = 0.0f;
    float end   = 4.0f;
    float delta = 0.00001f;
    float result;

    printf("start = %f, end = %f\n", start, end);

    result = calc_integral(start, end, delta);

    printf("result = %f\n", result);

    return 0;
}

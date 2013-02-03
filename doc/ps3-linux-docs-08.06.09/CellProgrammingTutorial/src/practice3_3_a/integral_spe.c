#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

typedef struct {
    float   start;
    float   end;
    float   delta;
    float   sum;
} integral_params_t;

integral_params_t integral_params __attribute__((aligned(16)));

__vector float func(__vector float vx)
{
    __vector float vy;
    __vector float va = (__vector float) {   5.0f,   5.0f,   5.0f,   5.0f };
    __vector float vb = (__vector float) { -16.0f, -16.0f, -16.0f, -16.0f };
    __vector float vc = (__vector float) { -36.0f, -36.0f, -36.0f, -36.0f };
    __vector float vd = (__vector float) {  64.0f,  64.0f,  64.0f,  64.0f };
    __vector float ve = (__vector float) { 192.0f, 192.0f, 192.0f, 192.0f };

    vy = spu_madd(va, vx, vb);
    vy = spu_madd(vy, vx, vc);
    vy = spu_madd(vy, vx, vd);
    vy = spu_madd(vy, vx, ve);

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
        vsum = spu_madd(func(vx), vdelta, vsum);
        vx   = spu_madd(vdelta, vstep, vx);
    }

    sum = (float *) &vsum;

    return (sum[0] + sum[1] + sum[2] + sum[3]);
}

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int tag = 1;

    /* DMA Transfer 1 : GET input/output parameters */
    spu_mfcdma64(&integral_params, mfc_ea2h(argp), mfc_ea2l(argp),
                 sizeof(integral_params_t), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    printf("[SPE] start = %f, end = %f\n", integral_params.start, integral_params.end);

    integral_params.sum = calc_integral(integral_params.start,
                                        integral_params.end,
                                        integral_params.delta);

    printf("[SPE] sum = %f\n", integral_params.sum);

    /* DMA Transfer 2 : PUT output data */
    spu_mfcdma64(&integral_params, mfc_ea2h(argp), mfc_ea2l(argp),
                 sizeof(integral_params_t), tag, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    return 0;
}

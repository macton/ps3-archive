#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#define MAX_BUFSIZE (128)

float in_spe[MAX_BUFSIZE]  __attribute__((aligned(16)));
float out_spe[MAX_BUFSIZE] __attribute__((aligned(16)));

typedef struct {
    unsigned long long  ea_in;
    unsigned long long  ea_out;
    unsigned int        size;
    int                 pad[3];
} abs_params_t;

abs_params_t abs_params __attribute__((aligned(16)));

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int i;
    int tag = 1;

    __vector float *vin  = (__vector float *) in_spe;
    __vector float *vout = (__vector float *) out_spe;
    __vector float vin_negative;
    __vector unsigned int vpat;

    __vector float vzero  = (__vector float) {  0,  0,  0,  0 };
    __vector float vminus = (__vector float) { -1, -1, -1, -1 };

    /* DMA Transfer 1 : GET input/output parameters */
    spu_mfcdma64(&abs_params, mfc_ea2h(argp), mfc_ea2l(argp),
                 sizeof(abs_params_t), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    /* DMA Transfer 2 : GET input data */
    spu_mfcdma64(vin, mfc_ea2h(abs_params.ea_in), mfc_ea2l(abs_params.ea_in),
                 abs_params.size * sizeof(float), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    /* Calculate absolute values with vector operation */
    for (i = 0; i < abs_params.size/4; i++) {
        vpat = spu_cmpgt(vin[i], vzero);
        vin_negative = spu_mul(vin[i], vminus);
        vout[i] = spu_sel(vin_negative, vin[i], vpat);
    }

    /* DMA Transfer 3 : PUT output data */
    spu_mfcdma64(vout, mfc_ea2h(abs_params.ea_out), mfc_ea2l(abs_params.ea_out),
                 abs_params.size * sizeof(float), tag, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    return 0;
}

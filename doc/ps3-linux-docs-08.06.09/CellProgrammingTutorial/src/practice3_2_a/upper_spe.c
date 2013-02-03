#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#define MAX_BUFSIZE (128)

char in_spe[MAX_BUFSIZE]  __attribute__((aligned(16)));
char out_spe[MAX_BUFSIZE] __attribute__((aligned(16)));

typedef struct {
    unsigned long long  ea_in;
    unsigned long long  ea_out;
    unsigned int        size;
    int                 pad[3];
} upper_params_t;

upper_params_t upper_params __attribute__((aligned(16)));

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int i;
    int tag = 1;

    __vector unsigned char *vin  = (__vector unsigned char *) in_spe;
    __vector unsigned char *vout = (__vector unsigned char *) out_spe;

    __vector unsigned char vout_upper;
    __vector unsigned char vpat, vpata, vpatz;

    __vector unsigned char va = spu_splats((unsigned char) 'a');
    __vector unsigned char vz = spu_splats((unsigned char) 'z');
    __vector unsigned char voffset = spu_splats((unsigned char) ('a' - 'A'));

    /* DMA Transfer 1 : GET input/output parameters */
    spu_mfcdma64(&upper_params, mfc_ea2h(argp), mfc_ea2l(argp),
                 sizeof(upper_params_t), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    /* DMA Transfer 2 : GET input data */
    spu_mfcdma64(vin, mfc_ea2h(upper_params.ea_in), mfc_ea2l(upper_params.ea_in),
                 upper_params.size, tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    printf("[SPE] in  = %s\n", in_spe);

    for (i = 0; i < upper_params.size/16; i++) {
        vpata = spu_cmpgt(va, vin[i]);
        vpatz = spu_cmpgt(vin[i], vz);
        vpat  = spu_or(vpata, vpatz);
        vout_upper = (__vector unsigned char)
            spu_sub((__vector unsigned short) vin[i], (__vector unsigned short) voffset);
        vout[i] = spu_sel(vout_upper, vin[i], vpat);
    }

    printf("[SPE] out = %s\n", out_spe);

    /* DMA Transfer 3 : PUT output data */
    spu_mfcdma64(vout, mfc_ea2h(upper_params.ea_out), mfc_ea2l(upper_params.ea_out),
                 upper_params.size, tag, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    return 0;
}

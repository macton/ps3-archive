#include <stdio.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#define MAX_BUFSIZE (16 << 10)

unsigned char src_spe[MAX_BUFSIZE] __attribute__((aligned(128)));
unsigned char dst_spe[MAX_BUFSIZE] __attribute__((aligned(128)));

typedef struct {
    unsigned long long  ea_src;
    unsigned long long  ea_dst;
    unsigned int        num;
    int                 pad[3];
} grayscale_params_t;

grayscale_params_t grayscale_params __attribute__((aligned(128)));

void rgb2y(unsigned char *src, unsigned char *dst, int num)
{
    int i;

    __vector unsigned char *vsrc = (__vector unsigned char *) src;
    __vector unsigned char *vdst = (__vector unsigned char *) dst;

    __vector unsigned int vr, vg, vb, vy, vpat;
    __vector float vfr, vfg, vfb, vfy;

    __vector float vrconst = spu_splats(0.29891f);
    __vector float vgconst = spu_splats(0.58661f);
    __vector float vbconst = spu_splats(0.11448f);
    __vector float vfzero  = spu_splats(0.0f);
    __vector unsigned int vmax = spu_splats((unsigned int) 255);

    __vector unsigned char vpatr = (__vector unsigned char) { 0x10, 0x10, 0x10, 0x00,
                                                              0x10, 0x10, 0x10, 0x04,
                                                              0x10, 0x10, 0x10, 0x08,
                                                              0x10, 0x10, 0x10, 0x0c };
    __vector unsigned char vpatg = (__vector unsigned char) { 0x10, 0x10, 0x10, 0x01,
                                                              0x10, 0x10, 0x10, 0x05,
                                                              0x10, 0x10, 0x10, 0x09,
                                                              0x10, 0x10, 0x10, 0x0d };
    __vector unsigned char vpatb = (__vector unsigned char) { 0x10, 0x10, 0x10, 0x02,
                                                              0x10, 0x10, 0x10, 0x06,
                                                              0x10, 0x10, 0x10, 0x0a,
                                                              0x10, 0x10, 0x10, 0x0e };
    __vector unsigned char vpaty = (__vector unsigned char) { 0x03, 0x03, 0x03, 0x10,
                                                              0x07, 0x07, 0x07, 0x10,
                                                              0x0b, 0x0b, 0x0b, 0x10,
                                                              0x0f, 0x0f, 0x0f, 0x10 };
    __vector unsigned char vzero = spu_splats((unsigned char) 0);

    for (i = 0; i < num/4; i++) {
        vr = (__vector unsigned int) spu_shuffle(vsrc[i], vzero, vpatr);
        vg = (__vector unsigned int) spu_shuffle(vsrc[i], vzero, vpatg);
        vb = (__vector unsigned int) spu_shuffle(vsrc[i], vzero, vpatb);

        vfr = spu_convtf(vr, 0);
        vfg = spu_convtf(vg, 0);
        vfb = spu_convtf(vb, 0);

        vfy = spu_madd(vfr, vrconst, vfzero);
        vfy = spu_madd(vfg, vgconst, vfy);
        vfy = spu_madd(vfb, vbconst, vfy);

        vy = spu_convtu(vfy, 0);

        vpat = spu_cmpgt(vy, vmax);
        vy = spu_sel(vy, vmax, vpat);

        vdst[i] = (__vector unsigned char) spu_shuffle(vy, (__vector unsigned int) vzero, vpaty);
    }

    return;
}

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int tag = 1;

    unsigned long long ea_src, ea_dst;
    unsigned int remain, num;

    /* DMA Transfer 1 : GET input/output parameters */
    spu_mfcdma64(&grayscale_params, mfc_ea2h(argp), mfc_ea2l(argp),
                 sizeof(grayscale_params_t), tag, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << tag);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    ea_src = grayscale_params.ea_src;
    ea_dst = grayscale_params.ea_dst;
    remain = grayscale_params.num;

    /* main loop */
    while (remain > 0) {
        if (remain > MAX_BUFSIZE/4) {
            num = MAX_BUFSIZE/4;
        } else {
            num = remain;
        }

        /* DMA Transfer 2 : GET input data */
        spu_mfcdma64(src_spe, mfc_ea2h(ea_src), mfc_ea2l(ea_src),
                     num * 4, tag, MFC_GET_CMD);
        spu_writech(MFC_WrTagMask, 1 << tag);
        spu_mfcstat(MFC_TAG_UPDATE_ALL);

        /* convert to grayscale data */
        rgb2y(src_spe, dst_spe, num);

        /* DMA Transfer 3 : PUT output data */
        spu_mfcdma64(dst_spe, mfc_ea2h(ea_dst), mfc_ea2l(ea_dst),
                     num * 4, tag, MFC_PUT_CMD);
        spu_writech(MFC_WrTagMask, 1 << tag);
        spu_mfcstat(MFC_TAG_UPDATE_ALL);

        remain -= num;
        ea_src += num * 4;
        ea_dst += num * 4;
    }

    return 0;
}

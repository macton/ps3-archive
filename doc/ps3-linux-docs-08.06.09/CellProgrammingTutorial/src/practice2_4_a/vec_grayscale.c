#include <stdio.h>
#include <altivec.h>

void rgb2y(unsigned char *src, unsigned char *dst, int num)
{
    int i;

    __vector unsigned char *vsrc = (__vector unsigned char *) src;
    __vector unsigned char *vdst = (__vector unsigned char *) dst;

    __vector unsigned int vr, vg, vb, vy, vpat;
    __vector float vfr, vfg, vfb, vfy;

    __vector float vrconst = (__vector float) { 0.29891f, 0.29891f, 0.29891f, 0.29891f };
    __vector float vgconst = (__vector float) { 0.58661f, 0.58661f, 0.58661f, 0.58661f };
    __vector float vbconst = (__vector float) { 0.11448f, 0.11448f, 0.11448f, 0.11448f };
    __vector float vfzero  = (__vector float) { 0.0f, 0.0f, 0.0f, 0.0f };
    __vector unsigned int vmax = (__vector unsigned int) { 255, 255, 255, 255 };

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
    __vector unsigned char vzero = (__vector unsigned char) { 0, 0, 0, 0, 0, 0, 0, 0,
                                                              0, 0, 0, 0, 0, 0, 0, 0 };

    for (i = 0; i < num/4; i++) {
        vr = (__vector unsigned int) vec_perm(vsrc[i], vzero, vpatr);
        vg = (__vector unsigned int) vec_perm(vsrc[i], vzero, vpatg);
        vb = (__vector unsigned int) vec_perm(vsrc[i], vzero, vpatb);

        vfr = vec_ctf(vr, 0);
        vfg = vec_ctf(vg, 0);
        vfb = vec_ctf(vb, 0);

        vfy = vec_madd(vfr, vrconst, vfzero);
        vfy = vec_madd(vfg, vgconst, vfy);
        vfy = vec_madd(vfb, vbconst, vfy);

        vy = vec_ctu(vfy, 0);

        vpat = vec_cmpgt(vy, vmax);
        vy = vec_sel(vy, vmax, vpat);

        vdst[i] = (__vector unsigned char) vec_perm(vy, (__vector unsigned int) vzero, vpaty);
    }

    return;
}

#include <spu_intrinsics.h>
#include <spu_mfcio.h>

int counter __attribute__((aligned(128)));

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int i;

    for (i = 0; i < 10000; i++) {
        spu_mfcdma64(&counter, mfc_ea2h(argp), mfc_ea2l(argp), sizeof(int), 0, MFC_GET_CMD);
        spu_writech(MFC_WrTagMask, 1 << 0);
        spu_mfcstat(MFC_TAG_UPDATE_ALL);

        counter++;

        spu_mfcdma64(&counter, mfc_ea2h(argp), mfc_ea2l(argp), sizeof(int), 0, MFC_PUT_CMD);
        spu_writech(MFC_WrTagMask, 1 << 0);
        spu_mfcstat(MFC_TAG_UPDATE_ALL);
    }

    return 0;
}

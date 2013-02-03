#include <spu_intrinsics.h>
#include <spu_mfcio.h>

int counter[32] __attribute__((aligned(128)));

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    int i;

    for (i = 0; i < 10000; i++) {
        do {
            spu_mfcdma64(counter, mfc_ea2h(argp), mfc_ea2l(argp), 128, 0, MFC_GETLLAR_CMD);
            spu_readch(MFC_RdAtomicStat);

            counter[0]++;

            spu_mfcdma64(counter, mfc_ea2h(argp), mfc_ea2l(argp), 128, 0, MFC_PUTLLC_CMD);
        } while(spu_readch(MFC_RdAtomicStat) & MFC_PUTLLC_STATUS);
    }

    return 0;
}

#include <stdio.h>
#include <string.h>
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

#define TOTALSIZE (1 << 24)
#define BUFSIZE   (1 << 14)

char ibuf[2][BUFSIZE] __attribute__((aligned(128)));
char obuf[2][BUFSIZE] __attribute__((aligned(128)));

unsigned long long ea_in, ea_out;
unsigned int itag[2] = { 0, 1 };
unsigned int otag[2] = { 2, 3 };

typedef struct {
    unsigned long long ea_in;
    unsigned long long ea_out;
} wbuf_params_t;

wbuf_params_t wbuf_params __attribute__((aligned(128)));

void double_buffer(void);
void initiate_dma_get_input(char*, unsigned int);
void initiate_dma_put_result(char*, unsigned int);
void wait_dma_completion(unsigned int);
void calculate(char*, char*);

int main(unsigned long long spe, unsigned long long argp, unsigned long long envp)
{
    /* Get input/output parameters */
    spu_mfcdma64(&wbuf_params, mfc_ea2h(argp), mfc_ea2l(argp), sizeof(wbuf_params_t), 0, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1 << 0);
    spu_mfcstat(MFC_TAG_UPDATE_ALL);

    ea_in  = wbuf_params.ea_in;
    ea_out = wbuf_params.ea_out;

    double_buffer();

    return 0;
}

void double_buffer(void)
{
    int i;
    int cur, next;

    cur = 0;
    next = cur ^ 1;

    initiate_dma_get_input(ibuf[cur], itag[cur]);

    for (i = 0; i < TOTALSIZE/BUFSIZE-1; i++) {
        initiate_dma_get_input(ibuf[next], itag[next]);

        wait_dma_completion((1 << itag[cur]) | (1 << otag[cur]));

        calculate(ibuf[cur], obuf[cur]);

        initiate_dma_put_result(obuf[cur], otag[cur]);

        cur = next;
        next = cur ^ 1;
    }

    wait_dma_completion((1 << itag[cur]) | (1 << otag[cur]));

    calculate(ibuf[cur], obuf[cur]);

    initiate_dma_put_result(obuf[cur], otag[cur]);

    wait_dma_completion((1 << itag[cur]) | (1 << otag[cur]));
}

void initiate_dma_get_input(char *buf, unsigned int tagid)
{
    spu_mfcdma64(buf, mfc_ea2h(ea_in), mfc_ea2l(ea_in), BUFSIZE, tagid, MFC_GET_CMD);
    ea_in += BUFSIZE;
}

void initiate_dma_put_result(char *buf, unsigned int tagid)
{
    spu_mfcdma64(buf, mfc_ea2h(ea_out), mfc_ea2l(ea_out), BUFSIZE, tagid, MFC_PUT_CMD);
    ea_out += BUFSIZE;
}

void wait_dma_completion(unsigned int mask)
{
    spu_writech(MFC_WrTagMask, mask); 
    spu_mfcstat(MFC_TAG_UPDATE_ALL);
}

void calculate(char *src, char *dst)
{
    memcpy(dst, src, BUFSIZE);
}

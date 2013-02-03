#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

#define TOTALSIZE (1 << 24)

typedef struct {
    unsigned long long ea_in;
    unsigned long long ea_out;
} wbuf_params_t;

wbuf_params_t wbuf_params __attribute__((aligned(128)));

int main(int argc, char **argv)
{
    int i, ret;
 
    spe_context_ptr_t spe;
    spe_program_handle_t *prog;
    unsigned int entry;
    spe_stop_info_t stop_info;

    char *in, *out;

    ret = posix_memalign((void **) &in, 128, TOTALSIZE);
    if (ret < 0) {
        perror("posix_memalign(in)");
        exit(1);
    }

    for (i = 0; i < TOTALSIZE; i++) {
        in[i] = rand() & 0xff;
    }

    ret = posix_memalign((void **) &out, 128, TOTALSIZE);
    if (ret < 0) {
        perror("posix_memalign(out)");
        exit(1);
    }

    prog = spe_image_open("wbuf_spe.elf");
    if (!prog) {
        perror("spe_image_open");
        exit(1);
    }

    spe = spe_context_create(0, NULL);
    if (!spe) {
        perror("spe_context_create");
        exit(1);
    }

    ret = spe_program_load(spe, prog);
    if (ret) {
        perror("spe_program_load");
        exit(1);
    }

    wbuf_params.ea_in  = (unsigned long) in;
    wbuf_params.ea_out = (unsigned long) out;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(spe, &entry, 0, &wbuf_params, NULL, &stop_info);
    if (ret < 0) {
        perror("spe_context_run");
        exit(1);
    }

    ret = spe_context_destroy(spe);
    if (ret < 0) {
        perror("spe_context_destroy");
        exit(1);
    }

    ret = spe_image_close(prog);
    if (ret) {
        perror("spe_image_close");
        exit(1);
    }

    for (i = 0; i < TOTALSIZE; i++) {
        if (out[i] != in[i]) {
            printf("error %d in:%d out:%d\n", i, in[i], out[i]);
            exit(1);
        }
    }

    return 0;
}

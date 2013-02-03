#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

typedef struct {
    unsigned long long  ea_src;
    unsigned long long  ea_dst;
    unsigned int        num;
    int                 pad[3];
} grayscale_params_t;

grayscale_params_t grayscale_params __attribute__((aligned(128)));

void rgb2y(unsigned char *src, unsigned char *dst, int num)
{
    int ret;

    spe_context_ptr_t spe;
    spe_program_handle_t *prog;
    unsigned int entry;
    spe_stop_info_t stop_info;

    prog = spe_image_open("vec_grayscale_spe.elf");
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

    grayscale_params.ea_src = (unsigned long) src;
    grayscale_params.ea_dst = (unsigned long) dst;
    grayscale_params.num    = num;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(spe, &entry, 0, &grayscale_params, NULL, &stop_info);
    if (ret < 0) {
        perror("spe_context_run");
        exit(1);
    }

    ret = spe_context_destroy(spe);
    if (ret) {
        perror("spe_context_destroy");
        exit(1);
    }

    ret = spe_image_close(prog);
    if (ret) {
        perror("spe_image_close");
        exit(1);
    }

    return;
}

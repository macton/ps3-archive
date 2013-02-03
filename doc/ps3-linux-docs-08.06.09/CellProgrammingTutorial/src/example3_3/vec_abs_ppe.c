#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

#define SIZE    (64)

float in[SIZE]  __attribute__((aligned(16))) = {  1,  -2,  3,  -4,  5,  -6,  7,  -8,
                                                  9, -10, 11, -12, 13, -14, 15, -16,
                                                 17, -18, 19, -20, 21, -22, 23, -24,
                                                 25, -26, 27, -28, 29, -30, 31, -32,
                                                 33, -34, 35, -36, 37, -38, 39, -40,
                                                 41, -42, 43, -44, 45, -46, 47, -48,
                                                 49, -50, 51, -52, 53, -54, 55, -56,
                                                 57, -58, 59, -60, 61, -62, 63, -64 };
float out[SIZE] __attribute__((aligned(16)));

typedef struct {
    unsigned long long  ea_in;
    unsigned long long  ea_out;
    unsigned int        size;
    int                 pad[3];
} abs_params_t;

abs_params_t abs_params __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    int i;
    int ret;

    spe_context_ptr_t spe;
    spe_program_handle_t *prog;
    unsigned int entry;
    spe_stop_info_t stop_info;

    prog = spe_image_open("vec_abs_spe.elf");
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

    abs_params.ea_in  = (unsigned long) in;
    abs_params.ea_out = (unsigned long) out;
    abs_params.size   = SIZE;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(spe, &entry, 0, &abs_params, NULL, &stop_info);
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

    for (i = 0; i < SIZE; i++) {
        printf("out[%02d]=%0.0f\n", i, out[i]);
    }

    return 0;
}

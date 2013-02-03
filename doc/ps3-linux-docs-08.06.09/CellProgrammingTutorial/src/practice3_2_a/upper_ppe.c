#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>

#define LENGTH  (48)

char in[LENGTH]  __attribute__((aligned(16))) = "A Quick Brown Fox Jumps Over The Lazy Dog.";
char out[LENGTH] __attribute__((aligned(16)));

typedef struct {
    unsigned long long  ea_in;
    unsigned long long  ea_out;
    unsigned int        size;
    int                 pad[3];
} upper_params_t;

upper_params_t upper_params __attribute__((aligned(16)));

int main(int argc, char **argv)
{
    int ret;

    spe_context_ptr_t spe;
    spe_program_handle_t *prog;
    unsigned int entry;
    spe_stop_info_t stop_info;

    prog = spe_image_open("upper_spe.elf");
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

    printf("[PPE] in  = %s\n", in);

    upper_params.ea_in  = (unsigned long) in;
    upper_params.ea_out = (unsigned long) out;
    upper_params.size   = LENGTH;    

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(spe, &entry, 0, &upper_params, NULL, &stop_info);
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

    printf("[PPE] out = %s\n", out);

    return 0;
}

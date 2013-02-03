#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>

#define NUM_SPE 4
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

abs_params_t abs_params[NUM_SPE] __attribute__((aligned(16)));

typedef struct {
    spe_context_ptr_t   spe;
    abs_params_t        *abs_params;
} thread_arg_t;

void *run_abs_spe(void *thread_arg)
{
    int ret;
    thread_arg_t *arg = (thread_arg_t *) thread_arg;
    unsigned int entry;
    spe_stop_info_t stop_info;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(arg->spe, &entry, 0, arg->abs_params, NULL, &stop_info);
    if (ret < 0) {
        perror("spe_context_run");
        return NULL;
    }

    return NULL;
}

int main(int argc, char **argv)
{
    int i;
    int ret;

    spe_program_handle_t *prog;
    spe_context_ptr_t spe[NUM_SPE];
    pthread_t thread[NUM_SPE];
    thread_arg_t arg[NUM_SPE];

    prog = spe_image_open("vec_abs_spe.elf");
    if (!prog) {
        perror("spe_image_open");
        exit(1);
    }

    for (i = 0; i < NUM_SPE; i++) {
        spe[i] = spe_context_create(0, NULL);
        if (!spe[i]) {
            perror("spe_context_create");
            exit(1);
        }

        ret = spe_program_load(spe[i], prog);
        if (ret) {
            perror("spe_program_load");
            exit(1);
        }
    }

    int size = SIZE/NUM_SPE;
    for (i = 0; i < NUM_SPE; i++) {
		abs_params[i].ea_in  = (unsigned long) &in[i*size];
		abs_params[i].ea_out = (unsigned long) &out[i*size];
		abs_params[i].size   = size;

        arg[i].spe = spe[i];
        arg[i].abs_params = &abs_params[i];

        ret = pthread_create(&thread[i], NULL, run_abs_spe, &arg[i]);
        if (ret) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (i = 0; i < NUM_SPE; i++) {
        pthread_join(thread[i], NULL);
        ret = spe_context_destroy(spe[i]);
        if (ret) {
            perror("spe_context_destroy");
            exit(1);
        }
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

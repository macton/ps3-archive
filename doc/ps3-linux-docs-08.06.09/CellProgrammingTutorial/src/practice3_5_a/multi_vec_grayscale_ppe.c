#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>

#define NUM_SPE 4

#define ALIGN4_UP(x) ((x) & 0x3) ? ((((x) >> 2) + 0x1) << 2) : (x)

typedef struct {
    unsigned long long  ea_src;
    unsigned long long  ea_dst;
    unsigned int        num;
    int                 pad[3];
} grayscale_params_t;

grayscale_params_t grayscale_params[NUM_SPE] __attribute__((aligned(128)));

typedef struct {
    spe_context_ptr_t   spe;
    grayscale_params_t  *grayscale_params;
} thread_arg_t;

void *run_grayscale_spe(void *thread_arg)
{
    int ret;
    thread_arg_t *arg = (thread_arg_t *) thread_arg;
    unsigned int entry;
    spe_stop_info_t stop_info;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(arg->spe, &entry, 0, arg->grayscale_params, NULL, &stop_info);
    if (ret < 0) {
        perror("spe_context_run");
        return NULL;
    }

    return NULL;
}

void rgb2y(unsigned char *src, unsigned char *dst, int num)
{
    int i;
    int ret;
    int remain;
    int chunk;

    spe_program_handle_t *prog;
    spe_context_ptr_t spe[NUM_SPE];
    pthread_t thread[NUM_SPE];
    thread_arg_t arg[NUM_SPE];

    prog = spe_image_open("vec_grayscale_spe.elf");
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

    remain = num;
    chunk = num/NUM_SPE;
    for (i = 0; i < NUM_SPE; i++) {
        grayscale_params[i].ea_src = (unsigned long) src;
        grayscale_params[i].ea_dst = (unsigned long) dst;
        if (remain > chunk) {
            grayscale_params[i].num = ALIGN4_UP(chunk);
        } else {
            grayscale_params[i].num = ALIGN4_UP(remain);
        }

        arg[i].spe = spe[i];
        arg[i].grayscale_params = &grayscale_params[i];

        ret = pthread_create(&thread[i], NULL, run_grayscale_spe, &arg[i]);
        if (ret) {
            perror("pthread_create");
            exit(1);
        }

        remain -= chunk;
        src    += (chunk * 4);
        dst    += (chunk * 4);
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

    return;
}

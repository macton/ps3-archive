#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>

#define NUM_SPE 4

typedef struct {
    float   start;
    float   end;
    float   delta;
    float   sum;
} integral_params_t;

integral_params_t integral_params[NUM_SPE] __attribute__((aligned(16)));

typedef struct {
    spe_context_ptr_t spe;
    integral_params_t *integral_params;
} thread_arg_t;

void *run_integral_spe(void *thread_arg)
{
    int ret;
    thread_arg_t *arg = (thread_arg_t *) thread_arg;
    unsigned int entry;
    spe_stop_info_t stop_info;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(arg->spe, &entry, 0, arg->integral_params, NULL, &stop_info);
    if (ret < 0) {
        perror("spe_context_run");
        return NULL;
    }

    return NULL;
}

float calc_integral(float start, float end, float delta)
{
    int i;
    int ret;
    float sum = 0.0f;

    spe_program_handle_t *prog;
    spe_context_ptr_t spe[NUM_SPE];
    pthread_t thread[NUM_SPE];
    thread_arg_t arg[NUM_SPE];

    prog = spe_image_open("integral_spe.elf");
    if (!prog) {
        perror("spe_image_open");
        exit(1);
    }

    for (i = 0; i < NUM_SPE; i++) {
        spe[i] = spe_context_create(0, NULL);
        if (!spe) {
            perror("spe_context_create");
            exit(1);
        }

        ret = spe_program_load(spe[i], prog);
        if (ret) {
            perror("spe_program_load");
            exit(1);
        }
    }

    for (i = 0; i < NUM_SPE; i++) {
        integral_params[i].start = start + (end-start)/NUM_SPE * i;
        integral_params[i].end   = start + (end-start)/NUM_SPE * (i+1);
        integral_params[i].delta = delta;
        integral_params[i].sum   = 0.0f;

        arg[i].spe = spe[i];
        arg[i].integral_params = &integral_params[i];

        ret = pthread_create(&thread[i], NULL, run_integral_spe, &arg[i]);
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

    for (i = 0; i < NUM_SPE; i++) {
        printf("[PPE] sum = %f\n", integral_params[i].sum);
        sum += integral_params[i].sum;
    }

    return sum;
}

int main(int argc, char **argv)
{
    float start = 0.0f;
    float end   = 4.0f;
    float delta = 0.00001f;
    float result;

    printf("[PPE] start = %f, end = %f\n", start, end);

    result = calc_integral(start, end, delta);

    printf("[PPE] result = %f\n", result);

    return 0;
}

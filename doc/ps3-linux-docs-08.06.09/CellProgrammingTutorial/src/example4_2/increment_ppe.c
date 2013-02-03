#include <stdio.h>
#include <stdlib.h>
#include <libspe2.h>
#include <pthread.h>

#define NUM_SPE   6
#define TOTALSIZE (1 << 24)

int counter __attribute__((aligned(128)));

void *run_increment_spe(void *thread_arg)
{
    int ret;
    unsigned int entry;
    spe_context_ptr_t *spe = (spe_context_ptr_t *) thread_arg;
    spe_stop_info_t stop_info;

    entry = SPE_DEFAULT_ENTRY;
    ret = spe_context_run(*spe, &entry, 0, &counter, NULL, &stop_info);
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
 
    spe_context_ptr_t spe[NUM_SPE];
    spe_program_handle_t *prog;
    pthread_t thread[NUM_SPE];

    prog = spe_image_open("increment_spe.elf");
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
        ret = pthread_create(&thread[i], NULL, run_increment_spe, &spe[i]);
        if (ret) {
            perror("pthread_create");
            exit(1);
        }
    }

    for (i = 0; i < NUM_SPE; i++) {
        pthread_join(thread[i], NULL);
        ret = spe_context_destroy(spe[i]);
        if (ret < 0) {
            perror("spe_context_destroy");
            exit(1);
        }
    }

    ret = spe_image_close(prog);
    if (ret) {
        perror("spe_image_close");
        exit(1);
    }

    printf("result=%d\n", counter);

    return 0;
}

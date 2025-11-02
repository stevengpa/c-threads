#define _GNU_SOURCE
#include <pthread.h>
#include <sched.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

// gcc -o thread c_thread_params.c -lpthread -D_GNU_SOURCE
// sudo ./thread

void set_cpu_affinity() {
    pthread_t thread;
    pthread_attr_t attr;
    cpu_set_t cpus;

    pthread_attr_init(&attr);
    CPU_ZERO(&cpus);
    CPU_SET(0, &cpus);  // Set thread to run on CPU 0

    pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
    pthread_create(&thread, &attr, thread_function, NULL);
    pthread_join(thread, NULL);
    pthread_attr_destroy(&attr);
}

void* thread_function(void* arg) {
    pthread_t tid = pthread_self();
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);

    if (pthread_getaffinity_np(tid, sizeof(cpu_set_t), &cpuset) == 0) {
        printf("Thread %lu running on CPUs: ", (unsigned long)tid);
        for (int i = 0; i < CPU_SETSIZE; i++) {
            if (CPU_ISSET(i, &cpuset)) {
                printf("%d ", i);
            }
        }
        printf("\n");
    }

    return NULL;
}

int main(int argc, char **argv) {
    pthread_t thread;
    pthread_attr_t attr;
    cpu_set_t cpuset;
    struct sched_param param;

    pthread_attr_init(&attr);

    size_t stacksize = 1024 * 1024;
    pthread_attr_setstacksize(&attr, stacksize);

    pthread_attr_setschedpolicy(&attr, SCHED_RR);

    param.sched_priority = sched_get_priority_max(SCHED_RR);
    pthread_attr_setschedparam(&attr, &param);
    pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);

    CPU_ZERO(&cpuset);
    CPU_SET(0, &cpuset);
    pthread_attr_setaffinity_np(&attr, CPU_SETSIZE, &cpuset);

    if (pthread_create(&thread, &attr, CPU_SETSIZE, &cpuset)) {
        perror("Failed to create thread");
        return 1;
    }

    pthread_join(thread, NULL);
    pthread_attr_destroy(&attr);

    return 0;
}
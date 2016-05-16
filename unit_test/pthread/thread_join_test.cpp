#include <pthread.h>
#include <stdio.h>

void * dostuff(void* arg) {
    printf("child thread\n");
    return NULL;
}

int main() {
    pthread_t p1;

    printf("before create\n");
    pthread_create(&p1, NULL, dostuff, NULL);
    printf("after create\n");

    pthread_join(p1, NULL);
    printf("joined\n");

    return 0;
}
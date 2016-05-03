#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

static int thread_running = 0;
static void* process(void* argc)
{
    while(thread_running)
    {
        double a = 1.0000;
        double b = 1.0000;
        double c = a/b;
        usleep(1000000);
        printf("usleep 1s %f\n", c);
    } 
    printf("process is exit \n");
}

int main()
{
    thread_running = 1;
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int res = pthread_create(&t_id, &attr, process, NULL);
    pthread_attr_destroy(&attr);
    int select = 0;
    while (1)
    {
        printf("please input your select !\n");
        scanf("%d", &select);
        if (select > 0)
        {
            thread_running = 1;
        }else {
            thread_running = 0;
            break;      
        }
    }
    usleep(1000000);
    printf("usleep(1000000) \n");
    pthread_join(t_id, NULL);
    return 0;        
}
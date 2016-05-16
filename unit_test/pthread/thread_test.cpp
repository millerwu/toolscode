#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>
#include <vector>

using namespace std;
static int thread_running = 0;
pthread_mutex_t _mutex;
pthread_cond_t _cond;
vector<int> test;
int index = 0;

static void* process(void* argc)
{
    while(thread_running)
    {
        printf("process thread while in\n");
        printf("pthread_mutex_lock\n");
        pthread_mutex_lock(&_mutex);
        double a = 1.0000;
        double b = 1.0000;
        double c = a/b;
        test.push_back(index++);
        pthread_cond_wait(&_cond, &_mutex); // it must be used with lock & unlock
        pthread_mutex_unlock(&_mutex);
        printf("pthread_mutex_unlock\n");
        usleep(100000);
        printf("usleep 1s %f\n", c);
    } 
    printf("process is exit \n");
}

int main(int argc, char** argv)
{
    thread_running = 1;
    pthread_t t_id;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    // pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    int res = pthread_create(&t_id, &attr, process, NULL);
    pthread_attr_destroy(&attr);
    pthread_mutex_init(&_mutex, NULL);
    pthread_cond_init(&_cond, NULL);
    int select = 0;
    while (1)
    {
        pthread_mutex_lock(&_mutex);
        test.push_back(index++);
        pthread_mutex_unlock(&_mutex);
        printf("please input your select !\n");
        scanf("%d", &select);
        if (select > 0)
        {
            pthread_cond_signal(&_cond);
            printf("signal \n");
        }else {
            thread_running = 0;
            break;
        }
    }
    usleep(1000000);
    printf("usleep(1000000) \n");
    pthread_join(t_id, NULL);
    pthread_mutex_destroy(&_mutex);
    pthread_cond_destroy(&_cond);
    return 0;        
}
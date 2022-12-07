#define _GNU_SOURCE        //这个必须定义在#include <sched.h>前

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<sys/time.h>
#include <sched.h>


#define APPLE_MAX_VALUE 100000000
#define ORANGE_MAX_VALUE 1000000
#define MSECOND 1000000

typedef struct 
{
    unsigned long long a;
    char c[256];
    unsigned long long b;
    pthread_rwlock_t rwLock;
}apple;

typedef struct
{
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
}orange;



void* apple_concurrent_a_mutex(void* ap);
void* apple_concurrent_b_mutex(void* ap);
void* orangeProcess(void* or);

void Concurrent_mutex();



int main(int argc, const char* argv[])
{
    struct timeval start[8], end[8];

    gettimeofday(&start[1], NULL);
    Concurrent_mutex();
    gettimeofday(&end[1], NULL);
    printf("%d: %ld\n", 1, 1000000*(end[1].tv_sec - start[1].tv_sec) + end[1].tv_usec - start[1].tv_usec);

    return 0;
}


void Concurrent_mutex()
{
    apple ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a_mutex, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b_mutex, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore); 
    pthread_join(apple_tid_1, NULL);
    pthread_join(apple_tid_2, NULL);
    pthread_join(orange_tid, NULL);
}



//******Third Thread*********************************************************************************
void* orangeProcess(void* or)
{
    int cpu_id = 1;            // 需要绑定的cpu
    cpu_set_t mask;            // cpu核的位掩码
    CPU_ZERO(&mask);           // 置空
    CPU_SET(cpu_id, &mask);    // 将需要绑定的cpu号设置在mask中
    orange ore = *((orange*)or);
    unsigned long long sum = 0;
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        sum += ore.a[cnt] + ore.b[cnt];
    }
}

void* apple_concurrent_a_mutex(void* ap)
{
    int cpu_id = 0;            // 需要绑定的cpu
    cpu_set_t mask;            // cpu核的位掩码
    CPU_ZERO(&mask);           // 置空
    CPU_SET(cpu_id, &mask);    // 将需要绑定的cpu号设置在mask中
    apple ape = *((apple*)ap);
    pthread_rwlock_wrlock(&ape.rwLock);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
    {
        ape.a += cnt;
    } 
    pthread_rwlock_unlock(&ape.rwLock);
    return NULL;
}

void* apple_concurrent_b_mutex(void* ap)
{
    int cpu_id = 1;            // 需要绑定的cpu
    cpu_set_t mask;            // cpu核的位掩码
    CPU_ZERO(&mask);           // 置空
    CPU_SET(cpu_id, &mask);    // 将需要绑定的cpu号设置在mask中
    apple ape = *((apple*)ap);
    pthread_rwlock_wrlock(&ape.rwLock);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        ape.b += cnt;
    }
    pthread_rwlock_unlock(&ape.rwLock);
    return NULL;
}

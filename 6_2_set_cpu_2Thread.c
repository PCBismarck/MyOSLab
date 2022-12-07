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
    unsigned long long b;
    pthread_rwlock_t rwLock;
}apple;

typedef struct
{
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
}orange;

typedef struct 
{
    unsigned long long a;
    char c[256];
    unsigned long long b;
    pthread_rwlock_t rwLock;
}apple_cache;

void* appleProcess(void* ap);
void* orangeProcess(void* or);

void Concurrent();



int main(int argc, const char* argv[])
{
    struct timeval start[8], end[8];

    gettimeofday(&start[1], NULL);
    Concurrent();
    gettimeofday(&end[1], NULL);
    printf("%d: %ld\n", 1, 1000000*(end[1].tv_sec - start[1].tv_sec) + end[1].tv_usec - start[1].tv_usec);

    return 0;
}


void Concurrent()
{
    long long iRet = 0;
    apple ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid, orange_tid;
    pthread_create(&apple_tid, NULL, &appleProcess, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore); 
    pthread_join(apple_tid, NULL);
    pthread_join(orange_tid, NULL);
    // printf("Concurrent end\n");
}



//******Two Thread*********************************************************************************
void* appleProcess(void* ap)
{
    int cpu_id = 0;            // 需要绑定的cpu
    cpu_set_t mask;            // cpu核的位掩码
    CPU_ZERO(&mask);           // 置空
    CPU_SET(cpu_id, &mask);    // 将需要绑定的cpu号设置在mask中
    apple ape = *((apple*)ap);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
    {
        ape.a += cnt;
        ape.b += cnt;
    }
    return NULL;
}

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


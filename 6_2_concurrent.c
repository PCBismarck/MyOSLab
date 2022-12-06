#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


#define APPLE_MAX_VALUE 100000000
#define ORANGE_MAX_VALUE 1000000
#define MSECOND 1000000

typedef struct 
{
    unsigned long long a;
    unsigned long long b;
    sem_t mutex;
}apple;

typedef struct
{
    int a[ORANGE_MAX_VALUE];
    int b[ORANGE_MAX_VALUE];
}orange;

void* apple_concurrent_a(void* ap);
void* apple_concurrent_b(void* ap);
void* apple_concurrent_a_mutex(void* ap);
void* apple_concurrent_b_mutex(void* ap);
void* appleProcess(void* ap);
void* orangeProcess(void* or);

void Serial();
void Concurrent();
void Concurrent_mutex();
void Concurrent_no_mutex();


int main(int argc, const char* argv[])
{
    if (argc == 1)
    {
        printf("Serial usage: 6_2_concurrent 1\n");
        printf("Concurrent usage: 6_2_concurrent 2\n");
        return -1;     
    }

    if (argv[1][0] == '1')
    {
        Serial();
    }
    else if (argv[1][0] == '2')
    {
        Concurrent();
    }
    else if (argv[1][0] == '3')
    {
        Concurrent_mutex();
    }    
    else if (argv[1][0] == '4')
    {
        Concurrent_no_mutex();
    }
    return 0;
}


void Serial()
{
    apple ape;
    orange ore;
    unsigned long long cnt, sum;

    for(cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)  
    {
        ape.a += cnt;
        ape.b += cnt;
    }

    for (cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        sum += ore.a[cnt] + ore.b[cnt];
    }
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

void Concurrent_mutex()
{
    long long iRet = 0;
    apple ape;
    orange ore;
    unsigned long long cnt;
    sem_init(&ape.mutex, 0, 1);
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a_mutex, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b_mutex, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore); 
    pthread_join(apple_tid_1, (void**)iRet);
    pthread_join(apple_tid_2, (void**)iRet);
    pthread_join(orange_tid, (void**)iRet);
}


void Concurrent_no_mutex()
{
    long long iRet = 0;
    apple ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore);
    pthread_join(apple_tid_1, (void**)iRet);
    pthread_join(apple_tid_2, (void**)iRet);
    pthread_join(orange_tid, (void**)iRet); 
}

void* appleProcess(void* ap)
{
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
    orange ore = *((orange*)or);
    unsigned long long sum = 0;
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        sum += ore.a[cnt] + ore.b[cnt];
    }
}

void* apple_concurrent_a(void* ap)
{
    apple ape = *((apple*)ap);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
        ape.a += cnt;    
    return NULL;
}

void* apple_concurrent_b(void* ap)
{
    apple ape = *((apple*)ap);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
        ape.b += cnt;
    return NULL;
}

void* apple_concurrent_a_mutex(void* ap)
{
    apple ape = *((apple*)ap);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
    {
        sem_wait(&ape.mutex);
        ape.a += cnt;
        sem_post(&ape.mutex);
    } 
    return NULL;
}

void* apple_concurrent_b_mutex(void* ap)
{
    apple ape = *((apple*)ap);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        sem_wait(&ape.mutex);
        ape.b += cnt;
        sem_post(&ape.mutex);
    }
    return NULL;
}


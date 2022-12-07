#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include<sys/time.h>


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

void* apple_concurrent_a(void* ap);
void* apple_concurrent_b(void* ap);
void* apple_concurrent_a_mutex(void* ap);
void* apple_concurrent_b_mutex(void* ap);
void* apple_concurrent_a_cache(void* ap);
void* apple_concurrent_b_cache(void* ap);
void* apple_concurrent_a_mutex_cache(void* ap);
void* apple_concurrent_b_mutex_cache(void* ap);
void* appleProcess(void* ap);
void* orangeProcess(void* or);

void Serial();
void Concurrent();
void Concurrent_mutex();
void Concurrent_no_mutex();
void Concurrent_mutex_cache();
void Concurrent_no_mutex_cache();


int main(int argc, const char* argv[])
{
    double timeRecord[8];
    struct timeval start[8], end[8];


    gettimeofday(&start[0], NULL);
    Serial();
    gettimeofday(&end[0], NULL);
    // printf("start:%ld  end:%ld \n", start[0].tv_usec, end[0].tv_usec);

    gettimeofday(&start[1], NULL);
    Concurrent();
    gettimeofday(&end[1], NULL);
    // printf("start:%ld  end:%ld \n", start[1].tv_usec, end[1].tv_usec);
    
    gettimeofday(&start[2], NULL);
    Concurrent_mutex();
    gettimeofday(&end[2], NULL);
    // printf("start:%ld  end:%ld \n", start[2].tv_usec, end[2].tv_usec);
    
    gettimeofday(&start[3], NULL);
    Concurrent_no_mutex();
    gettimeofday(&end[3], NULL);
    // printf("start:%ld  end:%ld \n", start[3].tv_usec, end[3].tv_usec);
    
    gettimeofday(&start[4], NULL);
    Concurrent_mutex_cache();
    gettimeofday(&end[4], NULL);
    // printf("start:%ld  end:%ld \n", start[4].tv_usec, end[4].tv_usec);
    
    gettimeofday(&start[5], NULL);
    Concurrent_no_mutex_cache();
    gettimeofday(&end[5], NULL);
    // printf("start:%ld  end:%ld \n", start[5].tv_usec, end[5].tv_usec);


    printf("0:serial  1:concurrent_2  2:concurrent_3_mutex  3:concurrent_3_noMutex  4:concurrent_3_mutex_cache  5:concurrent_3_noMutex_cache\n");
    for (int i = 0; i < 6; ++i)
    {
        printf("%d: %ld\n", i, 1000000*(end[i].tv_sec - start[i].tv_sec) + end[i].tv_usec - start[i].tv_usec);
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


void Concurrent_no_mutex()
{
    apple ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore);
    pthread_join(apple_tid_1, NULL);
    pthread_join(apple_tid_2, NULL);
    pthread_join(orange_tid, NULL); 
}

void Concurrent_mutex_cache()
{
    apple_cache ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a_mutex_cache, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b_mutex_cache, &ape);     
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore); 
    pthread_join(apple_tid_1, NULL);
    pthread_join(apple_tid_2, NULL);
    pthread_join(orange_tid, NULL);
}


void Concurrent_no_mutex_cache()
{
    apple_cache ape;
    orange ore;
    unsigned long long cnt;
    pthread_t apple_tid_1, apple_tid_2, orange_tid;
    pthread_create(&apple_tid_1, NULL, &apple_concurrent_a_cache, &ape);
    pthread_create(&apple_tid_2, NULL, &apple_concurrent_b_cache, &ape);    
    pthread_create(&orange_tid, NULL, &orangeProcess, &ore);
    pthread_join(apple_tid_1, NULL);
    pthread_join(apple_tid_2, NULL);
    pthread_join(orange_tid, NULL); 
}


//******Two Thread*********************************************************************************
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
//******Third Thread****************************************************************************
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
    apple ape = *((apple*)ap);
     pthread_rwlock_wrlock(&ape.rwLock);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        ape.b += cnt;
    }
    pthread_rwlock_unlock(&ape.rwLock);
    return NULL;
}

//**************cache**********************************

void* apple_concurrent_a_cache(void* ap)
{
    apple_cache ape = *((apple_cache*)ap);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
        ape.a += cnt;    
    return NULL;
}

void* apple_concurrent_b_cache(void* ap)
{
    apple_cache ape = *((apple_cache*)ap);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
        ape.b += cnt;
    return NULL;
}

void* apple_concurrent_a_mutex_cache(void* ap)
{
    apple_cache ape = *((apple_cache*)ap);
    pthread_rwlock_wrlock(&ape.rwLock);
    for(unsigned long long cnt = 0; cnt < APPLE_MAX_VALUE; ++cnt)
    {
        ape.a += cnt;
    } 
    pthread_rwlock_unlock(&ape.rwLock);
    return NULL;
}

void* apple_concurrent_b_mutex_cache(void* ap)
{
    apple_cache ape = *((apple_cache*)ap);
    pthread_rwlock_wrlock(&ape.rwLock);
    for(unsigned long long cnt = 0; cnt < ORANGE_MAX_VALUE; ++cnt)
    {
        ape.b += cnt;
    }
    pthread_rwlock_unlock(&ape.rwLock);
    return NULL;
}


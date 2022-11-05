#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>


#define DOCTOR_NUM 3
#define PATIENT_NUM 30
#define WORKING 1
#define RELAXING 0
#define EMPTY 0
#define WATING 1
#define TREATED 2


sem_t DMutex, PMutex, IMutex, RMutex, TMutex;
int DState[DOCTOR_NUM] = {0};//0 休息  1工作
int PState[PATIENT_NUM] = {0};//
int Reserve = 0, Treated = 0;


void* doctor(void* arg)
{
    int seq = *((int*)arg);
    sem_post(&IMutex);
    int working = 0;
    sleep(0.7);
    while (1)
    {   
        sem_wait(&DMutex);
        if(DState[seq] == RELAXING)
        {
            sem_wait(&PMutex);
            int i;
            for(i= 0; i < PATIENT_NUM && PState[i] != WATING; ++i);
            if(i < PATIENT_NUM)
            {
                working = 1;
                printf("Doc %d start treating patient %d!\n", seq+1, i+1);
                PState[i] = TREATED;
                DState[seq] = WORKING;
            }
            sem_post(&PMutex);
        }
        else
        {
            working = 0;
            DState[seq] = RELAXING;
            printf("Doc %d finish treating\n", seq+1);
            
            sem_wait(&TMutex);
            Treated++;
            if(Treated >= PATIENT_NUM)
            {
                sem_post(&DMutex);
                sem_post(&TMutex);
                break;
            }
            sem_post(&TMutex);
        }

        sem_post(&DMutex);
        if(working)
        {
            sleep(0.7);
        }
    }
    return 0;
}

void* patient(void* arg)
{
    int seq = *((int*)arg);
    sem_post(&IMutex);
    sem_wait(&PMutex);
    int i;
    for(i = 0 ; i < PATIENT_NUM && PState[i] != EMPTY; ++i);
    if(i < PATIENT_NUM)
    {
        PState[i] = WATING;
        sem_wait(&RMutex);
        Reserve++;
        sem_post(&RMutex);   
        printf("Patient %d get reserve num: %d\n", seq+1, i+1);
    }
    else
        printf("Patient %d leave\n", seq+1);
    sem_post(&PMutex);
    return 0;
}

int main(void)
{
    sem_init(&DMutex, 0, 1);
    sem_init(&PMutex, 0, 1);
    sem_init(&IMutex, 0, 1);
    sem_init(&RMutex, 0, 1);
    sem_init(&TMutex, 0, 1);
    pthread_t tid;
    int err;
    for(int i = 0 ; i < DOCTOR_NUM; ++i)
    {
        sem_wait(&IMutex);
        int seq = i;
        err = pthread_create(&tid, NULL, &doctor, &seq);
        if(err)
        {
            printf("create doc%d fail\n", i);
        }
    }
    for(int i = 0 ; i < PATIENT_NUM*2; ++i)
    {
        sem_wait(&IMutex);
        int seq = i;
        err = pthread_create(&tid, NULL, &patient, &seq);
        if(err)
        {
            printf("create patient%d fail\n", i);
        }
    }
    while (1)
    {
        sleep(1);
        sem_wait(&TMutex);
        if(Treated >= PATIENT_NUM)
        {
            sem_post(&TMutex);
            break;
        }
        sem_post(&TMutex);
    }
    
    return 0;
}
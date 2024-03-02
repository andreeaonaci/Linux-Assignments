#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include "a2_helper.h"

pthread_t threads[6], threads1[6];
pthread_t threads_barrier[48];
sem_t sem5_p4, sem5_p3, *sem_5_1, sem8, *sem8_4, sem, thread12_sem6, thread13_sem6, thread14_sem6, thread15_sem6;

void *thread_create_no_sem_wait()
{
    pthread_exit(NULL);
}

void *thread_create_3(void *arg)
{
    info(BEGIN, 5, 3);
    sem_wait(&sem5_p3);
    sem_post(&sem5_p4);
    pthread_exit(NULL);
}

void *thread_create_4(void *arg)
{
    sem_wait(&sem5_p4);
    info(BEGIN, 5, 4);
    sem_post(&sem5_p3);
    pthread_exit(NULL);
}

void *thread_create(void *arg)
{
    int thread_nr = (int)(long)arg;
    if (thread_nr == 1)
    {
        sem_wait(sem_5_1);
    }

    info(BEGIN, 5, thread_nr);

    info(END, 5, thread_nr);
    if (thread_nr == 1)
        sem_post(sem8_4);
    pthread_exit(NULL);
}

void *thread_create_process4(void *arg2)
{
    int thread_nr = (int)(long)arg2;
    if (thread_nr != 0) {
    if (thread_nr == 4)
        sem_wait(sem8_4);
    info(BEGIN, 8, thread_nr);
    info(END, 8, thread_nr);
    if (thread_nr == 1) {
        sem_post(sem_5_1);
    }
    }
    else
    {
        info(END, 8, 0);
    }
    pthread_exit(NULL);
}

int process4()
{
    sem_init(&sem5_p3, 0, 1);
    sem_init(&sem5_p4, 0, 0);
    
    sem_5_1 = sem_open("sem5_p1", O_CREAT, 0777, 0);
    sem8_4 = sem_open("sem8_4", O_CREAT, 0777, 0);

    pthread_create(&threads[0], NULL, thread_create_process4, (void *)0);
    pthread_create(&threads[1], NULL, thread_create_process4, (void *)1);
    pthread_create(&threads[2], NULL, thread_create_process4, (void *)2);
    pthread_create(&threads[3], NULL, thread_create_process4, (void *)3);
    
    pthread_create(&threads[4], NULL, thread_create_process4, (void *)4);
    pthread_create(&threads[5], NULL, thread_create_process4, (void *)5);
    pthread_create(&threads1[1], NULL, thread_create, (void *)1);
    pthread_create(&threads1[2], NULL, thread_create, (void *)2);
    pthread_create(&threads1[5], NULL, thread_create, (void *)5);
    pthread_create(&threads1[3], NULL, thread_create_3, (void *)3);
    pthread_create(&threads1[4], NULL, thread_create_4, (void *)4);

    for (int i = 0; i < 6; i++)
    {
        pthread_join(threads[i], NULL);
    }

    info(END, 5, 4);
    info(END, 5, 3);

    for (int i = 0; i < 6; i++)
    {
        pthread_join(threads1[i], NULL);
    }

    sem_destroy(&sem5_p3);
    sem_destroy(&sem5_p4);
    return 0;
}

sem_t sem6, thread12_sem6, sem13, sem14, sem15;

void *thread_create_barrier(void *arg)
{
    int thread_nr = *(int *)arg;

    if (thread_nr != 0)
    {
        info(BEGIN, 6, thread_nr);
    }

    sem_wait(&sem6);

    if (thread_nr == 12)
    {
        sem_post(&thread12_sem6);
        
        sem_wait(&sem13);
        sem_wait(&sem14);
        sem_wait(&sem15);
        info(END, 6, 12);
        info(END, 6, 13);
        info(END, 6, 14);
        info(END, 6, 15);
    }
    else if (thread_nr == 13)
    {
        sem_post(&sem13);
    }
    else if (thread_nr == 14)
    {
        sem_post(&sem14);
    }
    else if (thread_nr == 15)
    {
        sem_post(&sem15);
    }

    sem_post(&sem6);

    if (thread_nr != 0 && thread_nr != 12 && thread_nr != 13 && thread_nr != 14 && thread_nr != 15)
    {
        info(END, 6, thread_nr);
    }

    pthread_exit(NULL);
}


int process_barrier()
{
    sem_init(&sem6, 0, 4);
    sem_init(&thread12_sem6, 0, 0);
    sem_init(&thread13_sem6, 0, 0);
    sem_init(&thread14_sem6, 0, 0);
    sem_init(&thread15_sem6, 0, 0);

    int thread_nums[48];

    for (int i = 0; i < 48; i++)
    {
        thread_nums[i] = i;
        pthread_create(&threads_barrier[i], NULL, thread_create_barrier, &thread_nums[i]);
        if ((i + 1) % 4 == 0)
        {
            for (int j = i - 3; j <= i; j++)
            {
                pthread_join(threads_barrier[j], NULL);
            }
            for (int j = i - 3; j <= i; j++)
            {
                sem_post(&sem6);
            }
        }
    }

    int remaining_threads = 0; // 48 % 4 (48 with main thread)
    if (remaining_threads != 0)
    {
        int start_index = 48 - remaining_threads;
        for (int j = start_index; j < 48; j++)
        {
            pthread_join(threads_barrier[j], NULL);
        }
        for (int j = start_index; j < 48; j++)
        {
            sem_post(&sem6);
        }
    }

    sem_wait(&thread12_sem6);

    sem_destroy(&sem6);
    sem_destroy(&thread12_sem6);
    sem_destroy(&thread13_sem6);
    sem_destroy(&thread14_sem6);
    sem_destroy(&thread15_sem6);

    return 0;
}

int p9()
{
    info(BEGIN, 9, 0);
    info(END, 9, 0);
    return 0;
}

int p5()
{
    info(BEGIN, 5, 0);
    info(END, 5, 0);
    return 0;
}

int p8()
{
    info(BEGIN, 8, 0);
    process4();
    return 0;
}

int p4()
{
    info(BEGIN, 4, 0);
    info(END, 4, 0);
    return 0;
}

int p2()
{
    info(BEGIN, 2, 0);
    int childPid, status;
    childPid = fork();
    if (childPid < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid == 0)
    {
        status = p8();
        return status;
    }
    waitpid(childPid, &status, 0);
    info(END, 2, 0);
    return 0;
}

int p3()
{
    info(BEGIN, 3, 0);
    int childPid, status;
    childPid = fork();
    if (childPid < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid == 0)
    {
        status = p4();
        return status;
    }
    waitpid(childPid, &status, 0);
    info(END, 3, 0);
    return 0;
}

int p6()
{
    info(BEGIN, 6, 0);
    int childPid, status;
    process_barrier();
    childPid = fork();
    if (childPid < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid == 0)
    {
        status = p9();
        return status;
    }
    waitpid(childPid, &status, 0);
    info(END, 6, 0);
    return 0;
}

int p7()
{
    info(BEGIN, 7, 0);
    info(END, 7, 0);
    return 0;
}

int p1()
{
    info(BEGIN, 1, 0);
    int childPid1, childPid2, childPid3, childPid4, childPid5;
    int status;
    childPid1 = fork();
    if (childPid1 < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid1 == 0)
    {
        status = p2();
        return status;
    }

    
    childPid2 = fork();

    if (childPid2 < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid2 == 0)
    {
        status = p3();
        return status;
    }

    
    childPid3 = fork();

    if (childPid3 < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid3 == 0)
    {
        status = p5();
        return status;
    }

    
    childPid4 = fork();

    if (childPid4 < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid4 == 0)
    {
        status = p6();
        return status;
    }

    
    childPid5 = fork();

    if (childPid5 < 0)
    {
        perror("Error creating new process");
        return 1;
    }
    if (childPid5 == 0)
    {
        status = p7();
        return status;
    }
    waitpid(childPid1, &status, 0);
    waitpid(childPid2, &status, 0);
    waitpid(childPid3, &status, 0);
    waitpid(childPid4, &status, 0);
    waitpid(childPid5, &status, 0);
    
    info(END, 1, 0);
    return 0;
}

int main()
{
    init();

    int status1;
    int pid1 = fork();

    if (pid1 < 0)
    {
        perror("Error creating new process");
        return 1;
    }

    if (pid1 == 0)
    {
        status1 = p1();
        return status1;
    }

    waitpid(pid1, &status1, 0);
    sem_close(sem_5_1);
    sem_close(sem8_4);
    unlink("/dev/shm/sem.sem_5_1");
    unlink("/dev/shm/sem.sem8_4");
    return 0;
}

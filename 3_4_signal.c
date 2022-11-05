#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
int flag_wait = 1;
void stop2()
{
    flag_wait = 0;
    printf("\nson interruption\n");
}
void stop()
{
    printf("\ninterruption\n");
}
int main()
{
    pid_t pid1, pid2;
    signal(3, stop);
    while ((pid1 = fork()) == -1)
        ;
    if (pid1 > 0)
    {
        while ((pid2 = fork()) == -1)
            ;
        if (pid2 > 0)
        {
            sleep(5);
            kill(pid1, 16);
            wait(0);
            kill(pid2, 17);
            wait(0);
            printf("\nParent process is killed\n");
            exit(0);
        }
        else
        {
            signal(17, stop2);
            while (flag_wait)
                ;
            printf("\nchild process 2 is killed\n");
            exit(0);
        }
    }
    else
    {
        signal(16, stop2);
        while (flag_wait)
            ;
        printf("\nchild process 1 is killed\n");
        exit(0);
    }
    return 0;
}
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void primes(int cur_pipe) __attribute__((noreturn));
void primes(int curPipe)
{
    int curPrime;

    if (read(curPipe, &curPrime, sizeof(curPrime)) == 0)
    {
        close(curPipe);
        exit(0);
    }
    
    printf("prime %d\n", curPrime);

    int fd[2];
    if (pipe(fd) < 0)
    {
        fprintf(2, "Pipe error\n");
        close(curPipe);
        exit(1);
    }

    int pid = fork();
    if (pid == 0)
    {
        close(fd[1]);
        close(curPipe);
        primes(fd[0]);
    }
    else
    {
        int num;
        close(fd[0]);
        while (read(curPipe, &num, sizeof(int)) > 0)
        {
            if (num % curPrime != 0)
            {
                if (write(fd[1], &num, sizeof(int)) != sizeof(int))
                {
                    fprintf(2, "Write error\n");
                    exit(1);
                }
            }
        }
        close(fd[1]);
        close(curPipe);
        wait(0);
        exit(0);
    }
}

int main()
{
    int fd[2];
    if (pipe(fd) < 0)
    {
        fprintf(2, "First pipe failed\n");
        exit(1);
    }

    int pid = fork();
    if (pid == 0)
    {
        close(fd[1]);
        primes(fd[0]);
    }
    else
    {
        close(fd[0]);
        for (int i = 2; i <= 280; i++)
        {
            if (write(fd[1], &i, sizeof(int)) != sizeof(int))
            {
                fprintf(2, "Write first pipe error\n");
                exit(1);
            }
        }
        close(fd[1]);
        wait(0);
    }
    exit(0);
}
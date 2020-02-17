#include "main.h"

int main()
{
    printPIDInfo();
    //создаём первый процесс
    int pid = fork();
    switch (pid)
    {
        case -1:
            perror("Fork");
            return -1;
        case 0:
        {
            //потомок
            execv("RS485_COM1", NULL);
            break;
        }
        default:
        {
            //родитель
           /* int status;
            waitpid(pid, &status, 0);
            printf("exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
            printf("child exitcode = %i\n", WEXITSTATUS(status));*/
            break;
        }

    }
    int pid2 = fork();
    switch (pid2)
    {
        case -1:
            perror("Fork");
            return -1;
        case 0:
        {
            //потомок
            execv("RS485_COM2", NULL);
            break;
        }
        default:
        {
            //родитель
           /* int status;
            waitpid(pid, &status, 0);
            printf("exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
            printf("child exitcode = %i\n", WEXITSTATUS(status));*/
            break;
        }

    }
    while (1)
    {        
        sleep(5);
        printf("Родитель здесь!\n");
    }
    
    return 0;
}

void printPIDInfo()
{
    printf("PID запущенного процесса %d \n", (int)getpid());
    printf("Родительский PID %d \n", (int)getppid());
}
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
            PIDCOM1 = getpid();
            printf ("Создан процесс для COM1: %d\n", PIDCOM1);
            if ( mkfifo(NAMEDPIPE_NAME, 0777) ) 
            {
                perror("error mkfifo\n");
                return 1;
            }
            printf("%s is created\n", NAMEDPIPE_NAME);
            
            execl("RS485_COM1", NAMEDPIPE_NAME, NULL);
            break;
        }
        default:
        {            
            //родитель
            usleep(5000);
            fdCOM1pipe = open(NAMEDPIPE_NAME, O_RDONLY | O_NONBLOCK);
            if (fdCOM1pipe <= 0)
            {
                perror("open read fifo\n");
                return -1;
            }
             printf("%s открыт для чтения: %d\n", NAMEDPIPE_NAME, 
                                                  fdCOM1pipe);
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
            perror("Fork\n");
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
    Rcom1Inint();
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

void Rcom1Inint()
{
    sigemptyset(&mask);//очищаем маску
    sigaddset(&mask, SIGRTMIN);//добавляем к маске SIGRTMIN

    Rcom.sa_flags = SA_SIGINFO;//делаем сигнал реального времени
    sigemptyset(&Rcom.sa_mask);

    Rcom.sa_sigaction = handlingDataCOM;
    if (sigaction(SIGRTMIN, &Rcom, NULL) == -1)
    {
        perror("установка обработчика на события COM\n");
        return 1;
    }
    
}

void handlingDataCOM(int signum, siginfo_t *siginfo, void *extra)
{
    int data_numb = siginfo->si_value.sival_int;
    //прочитать поочерёдно всё очереди
    printf("Сигнал от %d. Получено байт: %d\n", 
            siginfo->si_pid, 
            data_numb);
    char bufs[data_numb];
    printf("Создан массив для хранения %d байт\n", data_numb);
    int res = read(fdCOM1pipe, bufs, data_numb);
    //close(fdCOM1pipe);
    printf("Считано из FIFO: %d\n", res);
    int i = 0;
    for (i = 0; i < res; i++)
      printf("%c ", bufs[i]);

    printf("\n");

}
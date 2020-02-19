#include "main.h"

int main()
{
    printPIDInfo();
    //создаём каналы для чтения и записи в COM порты
    if( access( PIPE_READ, F_OK ) != -1 ) 
    {
        // file exists
    } 
    else 
    {
        if ( mkfifo(PIPE_READ, 0777) ) 
        {
            perror("error mkfifo read\n");
            return 1;
        }
    }   
    
    if( access( PIPE_WRITE, F_OK ) != -1 ) 
    {
        // file exists
    } 
    else 
    {
        if ( mkfifo(PIPE_WRITE, 0777) ) 
        {
            perror("error mkfifo write\n");
            return 1;
        }
    }
    printf("%s & %s is created\n", PIPE_READ, PIPE_WRITE);

    //открываем канал для чтения от потомка
    fdCOMpipeR = open(PIPE_READ, O_RDONLY | O_NONBLOCK);
    if (fdCOMpipeR <= 0)
    {
        perror("open read fifo\n");
        return -1;
    }
    
    printf("%s открыт для чтения: %d\n", 
           PIPE_READ, 
           fdCOMpipeR);
    //создаём первый процесс
    PIDCOM1 = fork();
    switch (PIDCOM1)
    {
        case -1:
            perror("Fork");
            return -1;
        case 0:
        {
            //потомок
            printf ("Создан процесс для COM1: %d\n", PIDCOM1);            
            execl("RS485_COM1", PIPE_READ, PIPE_WRITE, NULL);
            break;
        }
        default:
        {            
            //родитель
           // usleep(5000);
            
           
           /* int status;
            waitpid(pid, &status, 0);
            printf("exit normally? %s\n", (WIFEXITED(status) ? "true" : "false"));
            printf("child exitcode = %i\n", WEXITSTATUS(status));*/
            break;
        }

    }
    
    PIDCOM2 = fork();
    switch (PIDCOM2)
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
    int procPID = siginfo->si_pid;
    printf("Сигнал от %d. Получено байт: %d\n", 
            procPID, 
            data_numb);
    char bufs[data_numb];
    printf("Создан массив для хранения %d байт\n", data_numb);
    int res = read(fdCOMpipeR, bufs, data_numb);
    //прочитать поочерёдно всё очереди
    // if (procPID == PIDCOM1)
    // {
    //     //операции обработки данных для COM1
    //     return;
    // }
    // if (procPID == PIDCOM2)
    // {
    //     //операции обработки данных для COM2
    //     return;
    // }
    
    //close(fdCOM1pipe);
    printf("Считано из FIFO: %d\n", res);
    int i = 0;
    for (i = 0; i < res; i++)
      printf("%c ", bufs[i]);

    printf("\n");

    writeDataCOM(PIDCOM1, bufs, res);

}

void writeDataCOM(int pid, const char *data, const int datanum)
{
    union sigval value;  
    value.sival_int = datanum;
    fdCOMpipeW = open(PIPE_WRITE, O_WRONLY | O_NONBLOCK);
    if (fdCOMpipeW <= 0)
    {
        perror("open write fifo\n");
        return -1;
    }
    printf("%s открыт для записи: %d\n", 
           PIPE_WRITE, 
           fdCOMpipeW);

    int res = write(fdCOMpipeW, data, datanum);
    printf("Записано в FIFO: %d\n", res);
    close(fdCOMpipeW);
    sigqueue(pid, SIGRTMIN+1, value);
    printf ("ОТправка сигнала к %d\n", pid);
}
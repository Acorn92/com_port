#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <signal.h>

#define PIPE_READ "my_read_pipe" 
#define PIPE_WRITE "my_write_pipe" 

int fdCOMpipeR, fdCOMpipeW = 0;
int PIDCOM1 = 0;
int PIDCOM2 = 0;
//печатает информацию об идентификаторах
void printPIDInfo();


struct sigaction Rcom;
sigset_t mask, oldmask;

//иниципализация обработчика сигналов от дочерних процессов
void Rcom1Inint();

//обработчик данных от COM
void handlingDataCOM(int signum, siginfo_t *siginfo, void *extra);
//отправка данных на запись в COM
void writeDataCOM(int pid, const char *data, const int datanum);
#endif
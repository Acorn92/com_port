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

#define NAMEDPIPE_NAME "my_named_pipe" 
int fdCOM1pipe = 0;
int PIDCOM1 = 0;
int PIDCOM2 = 0;
//печатает информацию об идентификаторах
void printPIDInfo();


struct sigaction Rcom;
sigset_t mask, oldmask;
siginfo_t siginfo;
struct timespec tv;

//иниципализация обработчика сигналов от дочерних процессов
void Rcom1Inint();

//обработчик данных от COM1
void handlingDataCOM(int signum, siginfo_t *siginfo, void *extra);

#endif
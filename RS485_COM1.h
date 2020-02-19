#ifndef RS485_COM1_H
#define RS485_COM1_H

#include <sys/stat.h>
#include <signal.h>
#include "com.h"


#define COM1 "/dev/ttyS2"
#define POSIX_SOURCE 1 //POSIX совместимое устройство
#define BAUDRATE B115200
#define COM1_ADRBASE 0x03E8 //IRQ4



#define TRUE 1
#define FALSE 0

#define DEBUG_PRINT = 1;

int wait_flag = TRUE; //TRUE пока не получен сигнал 

volatile int STOP=FALSE; 

COM_PORT portS2;

char *read_fifoN;//имя канала для принимаемых данных
char *write_fifoN;//имя канала для отправляемых данных

int PPID = 0;//идентификатор родительского процесса
//дескриптор файла именованного канала чтения и записи
int fdFIFOR, fdFIFOW;

void signal_handler_IO (int status); //обработчик сигнала
void signal_handler_TIMOUT(int status);

//отправка данных на запись в COM
void writeDatatoPort(int signum, siginfo_t *siginfo, void *extra);

#endif

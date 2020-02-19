#ifndef COMH
#define COMH

#define POSIX_SOURCE 1 //POSIX совместимое устройство


#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <time.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <signal.h>

//#include <math.h>

typedef struct port_t
{
    int port_d;//дескриптор порта
    char *port_name;//адрес порта в системе
    int baudrate;//скорость
    char echo_block;//блокировка эха
    char write_block;//блокировка записи, в случае, если идёт чтение
    struct termios oldtio, newtio;//структура хранящая настройка порта
     //объявление действия сигнала (signal action)
    // saio - сигнал наличия данных на приёмнике
    // satim - сигнал таймера эха
    // sawt - сигнал готовности данных на чтение
    struct sigaction saio, satim, sawr;
    //маска для сигнала реального времени sawr
    sigset_t mask;

    //struct itimerspec timer; //таймер для отсчёта времени блокировки приёмника
    struct itimerval timer; //таймер для отсчёта времени блокировки приёмника
    void (*read_handler)(int);//указатель на обработчик чтения
    void (*timer_handler)(int);//указатель на обработчик 
    void (*write_handler)(int, siginfo_t *, void *);//указатель на обработчик запиис в канал
} COM_PORT;

//инициализация порта
void typeInit(COM_PORT *port, char *__name, int __baudrate, 
              void (*__p)(int), void (*__timer_h)(int), 
              void (*__write)(int, siginfo_t *, void *));

//открывает COM порта для записи и чтения, не блокируемый,
//с подключением сигнала SIGIO
int openPort(COM_PORT *port);

//отправка данных в COM-порт, устанавливает флаг блокировки эха
int writePort(COM_PORT *port, const char *data, char numb_data);

//расчёт времени для передачи байтов(в миллисекундах)
int secCount(int __byteCount);

//обработчик таймера
void signal_handler_TIMOUT(int status);
#endif
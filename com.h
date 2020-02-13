#ifndef COMH
#define COMH

#define POSIX_SOURCE 1 //POSIX совместимое устройство


#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

//#include <math.h>

typedef struct port_t
{
    int port_d;//дескриптор порта
    char *port_name;//адрес порта в системе
    int baudrate;//скорость
    char echo_block;//блокировка эха
    struct termios oldtio, newtio;//структура хранящая настройка порта
    struct sigaction saio, satim; //объявление действия сигнала (signal action)
    struct itimerval timer; //таймер для отсчёта времени блокировки приёмника
    void (*read_handler)(int);//указатель на обработчик чтения
    void (*timer_handler)(int);//указатель на обработчик чтения
} COM_PORT;

//инициализация порта
void typeInit(COM_PORT *port, char *__name, int __baudrate, 
              void (*__p)(int), void (*__timer_h)(int));

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
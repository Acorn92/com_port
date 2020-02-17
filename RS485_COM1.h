#ifndef RS485_COM1_H
#define RS485_COM1_H

#include "com.h"

#define COM1 "/dev/ttyS2"
#define POSIX_SOURCE 1 //POSIX совместимое устройство
#define BAUDRATE B115200
#define COM1_ADRBASE 0x03E8 //IRQ4



#define TRUE 1
#define FALSE 0

#define DEBUG_PRINT = 1;

void signal_handler_IO (int status); //обработчик сигнала
void signal_handler_TIMOUT(int status);

#endif

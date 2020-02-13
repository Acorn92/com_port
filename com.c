#include "com.h"


#define DEBUG_PRINT = 1;

int openPort(COM_PORT *port)
{
    #ifdef DEBUG_PRINT
        printf("function: openPort %s", port->port_name);
    #endif
    //открываем порт
    port->port_d = open(port->port_name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    
    #ifdef DEBUG_PRINT
        printf("function: open %s", port->port_name);
    #endif

    //проверка успешности
    if (port->port_d < 0)
    {
        #ifdef DEBUG_PRINT
            printf("Ошибка открытия порта %s", port->port_name);
        #endif
        perror(port->port_name);
        exit(-1);
    }

    #ifdef DEBUG_PRINT
        printf("Порт %s успешно открыт", port->port_name);
    #endif

    //настройка сигнала
    port->saio.sa_handler = port->read_handler;    
    port->saio.sa_flags = 0;
    port->saio.sa_restorer = NULL;
    sigaction(SIGIO, &port->saio, NULL);

    //подключение сигнала
    fcntl(port->port_d, F_SETOWN, getgid());
    fcntl(port->port_d, F_SETFL, FASYNC);

    //считываем старые параметры порта
    tcgetattr(port->port_d, &port->oldtio);

    //установка новых параметров порта
    bzero(&port->newtio, sizeof(port->newtio));
    port->newtio.c_cflag = port->baudrate | CS8 | CLOCAL | CREAD;
    port->newtio.c_iflag = 0;
    port->newtio.c_oflag = 0;
    port->newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL);
    port->newtio.c_cc[VTIME] = 5; //установка таймаута для понимания конца посылкаи(n*0.1s)
    port->newtio.c_cc[VMIN] = 0; //блокировать чтение пока не будет принят 1 байт

    tcflush(port->port_d, TCIFLUSH);//очистка линии
    tcsetattr(port->port_d, TCSANOW, &port->newtio);//активация новых параметров

    return 0;
}

void typeInit(COM_PORT *port, char *__name, int __baudrate, void (*__p)(int))
{
    port->read_handler = __p;
    port->port_name = __name;
    port->baudrate = __baudrate;  
    #ifdef DEBUG_PRINT 
        printf("Проведена инициализация структура порта\n"); 
    #endif
}

int writePort(COM_PORT *port, const char *data, char numb_data)
{
    int res = 0;
    res = write(port->port_d, data, numb_data);
    port->echo_block = 1;//установка флага наличия эхо в порте
}

int secCount(int __byteCount)
{
    const int d = 8;
    const int s = 1;
    const int p = 0;
    const int baud = 115200;
    //d - количество бит данных - 8
    //s - количество стоповых бит - 1
    //p - количество бит четности - 0

    float res = 0;
    #ifdef DEBUG_PRINT 
        printf("Количество данных: %d\n", __byteCount); 
    #endif
    res = (float)(8 * __byteCount * (d + 1 + p + s)) / (float)(d * baud);
    res = res * 1000;
    
     #ifdef DEBUG_PRINT 
        printf("Время передачи данных: %d мс\n", (int)res); 
    #endif

    return (int)res;
}
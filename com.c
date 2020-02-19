#include "com.h"


#define DEBUG_PRINT = 1;

int openPort(COM_PORT *port)
{
    //открываем порт
    port->port_d = open(port->port_name, O_RDWR | O_NOCTTY | O_NONBLOCK | O_NDELAY);
    //проверка успешности
    if (port->port_d < 0)
    {
        #ifdef DEBUG_PRINT
            printf("Ошибка открытия порта %s\n", port->port_name);
        #endif
        perror(port->port_name);
        exit(-1);
    }
    fcntl(port->port_d, F_SETFL, 0);//очистка флагов состояний

    //настройка сигнала
    port->saio.sa_handler = port->read_handler;    
    port->saio.sa_flags = 0;
    port->saio.sa_restorer = NULL;
    sigaction(SIGIO, &port->saio, NULL);

    //настройка сигнала таймера
    memset(&port->satim, 0, sizeof(port->satim));
    port->satim.sa_handler = port->timer_handler;
    sigaction(SIGALRM, &port->satim, NULL);
    port->timer.it_value.tv_sec = 0;       
    port->timer.it_interval.tv_sec = 0;

    //настройка сигнала реального времени для передачи данных на чтение
    sigemptyset(&port->mask);//очищаем маску
    sigaddset(&port->mask, SIGRTMIN+1);//добавляем к маске SIGRTMIN

    port->sawr.sa_flags = SA_SIGINFO;//делаем сигнал реального времени
    sigemptyset(&port->sawr.sa_mask);

    port->sawr.sa_sigaction = port->write_handler;
    if (sigaction(SIGRTMIN+1, &port->sawr, NULL) == -1)
    {
        perror("установка обработчика на события COM\n");
        return 1;
    }
   

    //подключение сигнала
    fcntl(port->port_d, F_SETOWN, getgid());
    fcntl(port->port_d, F_SETFL, FASYNC);         

    //считываем старые параметры порта
    tcgetattr(port->port_d, &port->oldtio);

    
    //установка новых параметров порта
    memset(&port->newtio, 0, sizeof(struct termios));
    if ((cfsetispeed(&port->newtio, port->baudrate) < 0) ||
        (cfsetospeed(&port->newtio, port->baudrate) < 0)) 
    {
        perror("Unable to set baudrate");
    }
    /*
    CREAD - включить прием
    CLOCAL - игнорировать управление линиями с помошью
    */
    port->newtio.c_cflag |= (CREAD | CLOCAL);
    /* CSIZE - маска размера символа */
    port->newtio.c_cflag &= ~CSIZE;
    /* CS8 - 8 битные символы */
    port->newtio.c_cflag |= CS8;
 
    /* CSTOPB - при 1 - два стоп бита, при 0 - один */
    port->newtio.c_cflag &= ~CSTOPB;
 
    /*
    ICANON - канонический режим
    ECHO - эхо принятых символов
    ECHOE - удаление предыдущих символов по ERASE, слов по WERASE
    ISIG - реагировать на управляющие символы остановки, выхода, прерывания
    */
    port->newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    /* INPCK - вкл. проверку четности */
    port->newtio.c_iflag &= ~(INPCK);
    /* OPOST - режим вывода по умолчанию */
    port->newtio.c_oflag &= ~OPOST;
 
    /*
    TCSANOW - примернить изменения сейчасже
    TCSADRAIN - применить после передачи всех текущих данных
    TCSAFLUSH - приемнить после окончания передачи, все принятые но не считанные данные очистить
    */
    if (tcsetattr(port->port_d, TCSANOW, &port->newtio) < 0)
    {
        perror("Unable to set port parameters");     
    }

    tcflush(port->port_d, TCIFLUSH);//очистка линии
    //tcsetattr(port->port_d, TCSANOW, &port->newtio);//активация новых параметров
    printf("Порт %s успешно открыт\n", port->port_name);
    return 0;
}

void typeInit(COM_PORT *port, char *__name, int __baudrate, 
              void (*__p)(int), void (*__timer_h)(int), 
              void (*__write)(int, siginfo_t *, void *))
{
    port->read_handler = __p;
    port->timer_handler = __timer_h;
    port->write_handler = __write;
    port->port_name = __name;
    port->baudrate = __baudrate;  
    port->echo_block = 0;
    port->write_block = 0;
    #ifdef DEBUG_PRINT 
        printf("Проведена инициализация структура порта %s\n", __name); 
    #endif
}

int writePort(COM_PORT *port, const char *data, char numb_data)
{
    int res = 0;
    res = write(port->port_d, data, numb_data);
    port->echo_block = 1;//установка флага наличия эхо в порте
    return res;
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


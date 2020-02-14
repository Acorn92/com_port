#include <sys/signal.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>

#include "com.h"

#define COM1 "/dev/ttyS2"
#define POSIX_SOURCE 1 //POSIX совместимое устройство
#define BAUDRATE B115200
#define COM1_ADRBASE 0x03E8 //IRQ4

#define COM2 "/dev/ttyS3"
#define COM2_ADRBASE 0x02E8 //IQR3

#define TRUE 1
#define FALSE 0

#define DEBUG_PRINT = 1;

void signal_handler_IO (int status); //обработчик сигнала
void signal_handler_TIMOUT(int status);

int wait_flag = TRUE; //TRUE пока не получен сигнал 

volatile int STOP=FALSE; 
__asm__(".symver realpath,realpath@GLIBC_2.11.3");

int portd = 0;//дескриптор порта 1
int count_read = 0;
char read_buf[255];
struct itimerval timer;
COM_PORT portS2;
int main()
{
  printf("Hello World!!!\n");

  typeInit(&portS2, COM1, BAUDRATE, signal_handler_IO, 
            signal_handler_TIMOUT);

  openPort(&portS2);

    // struct termios oldtio, newtio;//структура хранящая настройка порта
    // struct sigaction saio, satim; //объявление действия сигнала (signal action)
    
    

    // portd = open(COM1, O_RDWR | O_NOCTTY | O_NONBLOCK);

    // if (portd < 0)
    // {
    //     perror(COM1);
    //     exit(-1);
    // }

    // saio.sa_handler = signal_handler_IO;
    // //saio.sa_mask = 0;
    // saio.sa_flags = 0;
    // saio.sa_restorer = NULL;
    // sigaction(SIGIO, &saio, NULL);


    // memset(&satim, 0, sizeof(satim));
    // satim.sa_handler = signal_handler_TIMOUT;
   
    // sigaction(SIGALRM, &satim, NULL);

    //  /* Таймер сработает через 250 миллисекунд... */

    // timer.it_value.tv_sec = 0;

    // timer.it_value.tv_usec = 500000;

    // /* ... и будет продолжать активизироваться каждые 250

    // миллисекунд. */

    // timer.it_interval.tv_sec = 0;

    // timer.it_interval.tv_usec = 500000;

   

    // fcntl(portd, F_SETOWN, getgid());

    // fcntl(portd, F_SETFL, FASYNC);

    // tcgetattr(portd, &oldtio);

    // bzero(&newtio, sizeof(newtio));
    // newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    // newtio.c_iflag = 0;
    // newtio.c_oflag = 0;
    // //newtio.c_lflag = 0;
    // newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL);
    // newtio.c_cc[VTIME] = 5; //установка таймаута для понимания конца посылкаи(n*0.1s)
    // newtio.c_cc[VMIN] = 0; //блокировать чтение пока не будет принят 1 байт

    // tcflush(portd, TCIFLUSH);
    // tcsetattr(portd, TCSANOW, &newtio);
    
        /*
          цикл ожидания ввода. обычно, мы будем делать здесь что-нибудь полезное
        */
        while (STOP==FALSE) 
        {
         // printf(".\n");usleep(100000);
          /*
            после получения SIGIO, wait_flag = FALSE, ввод доступен
            и может быть прочитан
          */
        //   if (wait_flag==FALSE) 
        //   { 
        //     res = read(portd,buf,255);
        //     buf[res]=0;
        //     printf(":%s:%d\n", buf, res);
        //     if (res==1) STOP=TRUE; /* останавливаем цикл при вводе символа CR */
        //     wait_flag = TRUE;      /* ждем нового воода */
        //   }
        }

  /*  char tmp[12] = "Hellow world\n";
    while (1)
    {
        write(portd, &tmp, 12);
        usleep(5000);
    }*/
    
    //tcsetattr (portd, TCSANOW, &oldtio);

    return 0;
}

void signal_handler_IO(int status)
{
  static echo_flag = 0;
  int bytes = 0;

  //если отправляли данные, флаг будет активен - сброс полученных данных в эхо
  if (!portS2.echo_block)
  {
    //если не это - принимаем данные
    ioctl(portS2.port_d, FIONREAD, &bytes);//получение количества считанных байт
    printf("принят сигнал SIGIO: %d\n", bytes);
    char buf[bytes];
    //(void)setitimer(ITIMER_REAL, NULL, NULL); /* выключить таймер */
    int res = read(portS2.port_d, &buf, bytes);//чтение   
  }

}

void writeDatatoPort(char *data, int data_numb)
{
  int res = writePort(&portS2, data, data_numb);//запись
  int time_delay = secCount(bytes)*2000;
  // чтобы не заблокировать цикл нулём, 
  // в случае приёма данных в маленьком количестве
  portS2.timer.it_value.tv_usec = time_delay >  0 ? time_delay : 2000;
  //portS2.timer.it_interval.tv_usec = 10000;/*secCount(bytes)*1000;*/

  (void)setitimer(ITIMER_REAL, &portS2.timer, NULL);
  bytes = 0;
  ioctl(portS2.port_d, FIONREAD, &bytes);
  printf("Данные отправлены. В буфере %d байт\n", bytes);
}

void signal_handler_TIMOUT(int status)
{
  #ifdef DEBUG_PRINT
        printf("Таймер!\n");
  #endif
  int bytes; 
  tcflush(portS2.port_d, TCIFLUSH);//очистка эхо
  ioctl(portS2.port_d, FIONREAD, &bytes);
    printf("В буфере %d байт\n", bytes);  
  (void)setitimer(ITIMER_REAL, NULL, NULL); /* выключить таймер */
  portS2.echo_block = 0;
}

//~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc -m32 -march=i586 main.c -o testserial -static-libgcc

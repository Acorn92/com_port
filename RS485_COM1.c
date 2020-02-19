//#include <sys/time.h>
#include "RS485_COM1.h"

__asm__(".symver realpath,realpath@GLIBC_2.11.3");

int main(char argc, char *argv[])
{
  PPID = (int)getppid();
  read_fifoN = argv[0];
  write_fifoN = argv[1];
  printf("%s, %s получены \n", read_fifoN, write_fifoN);
  //TODO: 
  //рассмотреть возможность перехода на разделяемую память

  printf("COM1 запущен. PID: %d\n", (int)getpid());
  printf("Родительский PID %d \n", (int)getppid());

  typeInit(&portS2, COM1, BAUDRATE, signal_handler_IO, 
            signal_handler_TIMOUT, writeDatatoPort);

  openPort(&portS2);

  fdFIFOW = open(write_fifoN, O_RDONLY | O_NONBLOCK);
  if (fdFIFOW <= 0)
  {
    perror("open read fifo\n");
    return -1;
  }

  printf("%s успешно открыт для чтения : %d\n", write_fifoN, fdFIFOW);

  while (1) 
  {
         
  }

  return 0;  
}

//обработка приёма данных с COM1
void signal_handler_IO(int status)
{
  union sigval value;  
  
  static echo_flag = 0;
  int bytes = 0;

  //если отправляли данные, флаг будет активен
  if (!portS2.echo_block)
  {
    //если не это - принимаем данные
    ioctl(portS2.port_d, FIONREAD, &bytes);//получение количества считанных байт
    printf("принят сигнал SIGIO: %d\n", bytes);
    char buf[bytes];
    //(void)setitimer(ITIMER_REAL, NULL, NULL); /* выключить таймер */
    int res = read(portS2.port_d, &buf, bytes);//чтение 
    int i = 0;
    for (i = 0; i < bytes; i++)
      printf("%c ", buf[i]);

    printf("\n");
    value.sival_int = bytes;
    //запись полученных данных в именованный канал
    fdFIFOR = open(read_fifoN, O_WRONLY | O_NONBLOCK);
    if (fdFIFOR <= 0)
    {
      perror("open fifo\n");
      return -1;
    }
     printf("%s открыт: %d\n", read_fifoN, fdFIFOR);
    //TODO: перед записью добавить подсчёт контрольной суммы 
    //и проверку целостности
    res = write(fdFIFOR, buf, bytes);
    printf("Записано в FIFO: %d\n", res);
    close(fdFIFOR);
    //отправка сигнала родителю, что данные прочитаны  
    sigqueue(PPID, SIGRTMIN, value);
    
  }

}
//функция отправки данных
//планируется вызываться по сигналу от основного процесса
void writeDatatoPort(int signum, siginfo_t *siginfo, void *extra)
{
  int data_numb = siginfo->si_value.sival_int;
  int procPID = siginfo->si_pid;
  printf("Сигнал от %d. Получено байт: %d\n", 
          procPID, 
          data_numb);
  char bufs[data_numb];
  printf("Создан массив для хранения %d байт\n", data_numb);
  int res = read(fdFIFOW, bufs, data_numb);
  printf("Прочитано данных из очереди: %d\n", res);
  int resw = writePort(&portS2, bufs, res);//запись
  printf("Записано данных в порт: %d\n", resw);
  int time_delay = secCount(resw)*2000;
  // чтобы не заблокировать цикл нулём, 
  // в случае приёма данных в маленьком количестве
  portS2.timer.it_value.tv_usec = time_delay >  0 ? time_delay : 2000;
  //portS2.timer.it_interval.tv_usec = 10000;/*secCount(bytes)*1000;*/
  (void)setitimer(ITIMER_REAL, &portS2.timer, NULL);
  int bytes = 0;
  ioctl(portS2.port_d, FIONREAD, &bytes);
  printf("Данные отправлены. В буфере %d байт\n", bytes);
}

//обработчик таймера на блокировку приёмника в момент работы передатчика
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

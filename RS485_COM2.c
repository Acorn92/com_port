#include "RS485_COM2.h"

int main()
{
    printf("COM2 открыт!");
    printf("Родительский PID %d \n", (int)getppid());
    
    return 0;
}
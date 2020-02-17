TARGET = testserial
CFLAGS = -m32 -march=i686 -static-libgcc

.PHONY: all clean

all: $(TARGET)

com.o: com.c com.h
	~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc $(CFLAGS) -c -o com.o com.c 

RS485_COM1.o : RS485_COM1.c RS485_COM1.h 
	~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc $(CFLAGS) -c -o RS485_COM1.o RS485_COM1.c 

RS485_COM2.o : RS485_COM2.c RS485_COM2.h 
	~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc $(CFLAGS) -c -o RS485_COM2.o RS485_COM2.c 

$(TARGET) : RS485_COM1.o com.o
	~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc $(CFLAGS) -o $(TARGET) RS485_COM1.o com.o

clean:
	rm -f *.o $(TARGET)*
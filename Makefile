CFLAGS = -m32 -march=i686 -static-libgcc

testserial : *.c 
	~/x-tools/i686-nptl-linux-gnu/bin/i686-nptl-linux-gnu-gcc $(CFLAGS) -o testserial *.c

clean:
	rm -f *.o testserial*
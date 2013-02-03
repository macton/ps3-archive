CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -maltivec -mabi=altivec -Wall

all: question3_4.elf

question3_4.elf: main.o question3_4.o
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $(CINCS) -c $^ -o $@

question3_4.o: question3_4.c
	$(CC) $(CFLAGS) $(CINCS) -c $^ -o $@

clean:
	rm -f *.o *.elf

CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -maltivec -mabi=altivec -Wall

all: question3_3.elf

question3_3.elf: question3_3.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

clean:
	rm -f *.elf

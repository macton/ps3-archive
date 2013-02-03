CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

all: question3_2.elf

question3_2.elf: question3_2.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

clean:
	rm -f *.elf

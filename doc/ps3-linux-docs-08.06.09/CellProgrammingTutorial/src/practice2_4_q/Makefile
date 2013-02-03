CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = main.c question2_4.c
OBJECT = main.o question2_4.o
TARGET = question2_4.elf

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECT): $(SOURCE)
	$(CC) $(CFLAGS) -c $(SOURCE)

clean:
	rm -f *.o *.elf

CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = main.c vec_grayscale.c
OBJECT = main.o vec_grayscale.o
TARGET = vec_grayscale.elf

$(TARGET): $(OBJECT)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJECT): $(SOURCE)
	$(CC) $(CFLAGS) -c $(SOURCE)

clean:
	rm -f *.o *.elf

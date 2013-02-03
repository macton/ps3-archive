CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = vec_abs.c
TARGET = vec_abs.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

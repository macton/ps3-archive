CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = vec_add.c
TARGET = vec_add.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

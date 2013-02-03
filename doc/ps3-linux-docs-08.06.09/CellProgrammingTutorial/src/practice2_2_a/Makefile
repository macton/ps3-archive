CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = vec_tenth.c
TARGET = vec_tenth.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

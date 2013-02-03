CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = abs_diff.c
TARGET = abs_diff.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

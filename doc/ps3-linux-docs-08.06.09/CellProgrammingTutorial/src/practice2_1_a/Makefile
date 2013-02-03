CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = vec_sum.c
TARGET = vec_sum.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

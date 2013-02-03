CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = mat_trans.c
TARGET = mat_trans.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

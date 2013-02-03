CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = question2_1.c
TARGET = question2_1.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

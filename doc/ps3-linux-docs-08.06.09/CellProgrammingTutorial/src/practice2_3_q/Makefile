CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = question2_3.c
TARGET = question2_3.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

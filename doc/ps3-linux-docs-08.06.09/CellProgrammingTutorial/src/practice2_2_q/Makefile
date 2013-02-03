CC     = gcc
CFLAGS = -maltivec -mabi=altivec

SOURCE = question2_2.c
TARGET = question2_2.elf

$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.elf

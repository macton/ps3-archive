CC         = gcc
CFLAGS     = -Wall
LDFLAGS    = -lspe2 -pthread

SPU_CC     = spu-gcc
SPU_CFLAGS = -Wall

all: increment_ppe.elf increment_spe.elf

increment_ppe.elf: increment_ppe.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

increment_spe.elf: increment_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $^ -o $@

clean:
	rm -f *.elf

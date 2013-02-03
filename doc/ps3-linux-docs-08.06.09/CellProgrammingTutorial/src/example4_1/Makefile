CC         = gcc
CFLAGS     = -Wall
LDFLAGS    = -lspe2

SPU_CC     = spu-gcc
SPU_CFLAGS = -Wall

all: wbuf_ppe.elf wbuf_spe.elf

wbuf_ppe.elf: wbuf_ppe.c
	$(CC) $(CFLAGS) $(LDFLAGS) $^ -o $@

wbuf_spe.elf: wbuf_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $^ -o $@

clean:
	rm -f *.elf

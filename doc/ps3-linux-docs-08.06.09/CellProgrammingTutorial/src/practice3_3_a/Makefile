CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

SPU_CC     = spu-gcc
SPU_CINCS  =
SPU_CLIBS  =
SPU_CFLAGS = -Wall

all: integral_ppe.elf integral_spe.elf

integral_ppe.elf: integral_ppe.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

integral_spe.elf: integral_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $(SPU_CINCS) $(SPU_CLIBS) $^ -o $@

clean:
	rm -f *.elf

CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

SPU_CC     = spu-gcc
SPU_CINCS  =
SPU_CLIBS  =
SPU_CFLAGS = -Wall

all: upper_ppe.elf upper_spe.elf

upper_ppe.elf: upper_ppe.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

upper_spe.elf: upper_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $(SPU_CINCS) $(SPU_CLIBS) $^ -o $@

clean:
	rm -f *.elf

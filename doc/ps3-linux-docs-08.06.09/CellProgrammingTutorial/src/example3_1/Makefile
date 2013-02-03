CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

SPU_CC     = spu-gcc
SPU_CINCS  =
SPU_CLIBS  =
SPU_CFLAGS = -Wall

all: hello_ppe.elf hello_spe.elf

hello_ppe.elf: hello_ppe.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

hello_spe.elf: hello_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $(SPU_CINCS) $(SPU_CLIBS) $^ -o $@

clean:
	rm -f *.elf

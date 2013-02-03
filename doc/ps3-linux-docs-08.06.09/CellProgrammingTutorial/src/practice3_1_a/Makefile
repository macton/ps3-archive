CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

SPU_CC     = spu-gcc
SPU_CINCS  =
SPU_CLIBS  =
SPU_CFLAGS = -Wall

all: print_param_ppe.elf print_param_spe.elf

print_param_ppe.elf: print_param_ppe.c
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

print_param_spe.elf: print_param_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $(SPU_CINCS) $(SPU_CLIBS) $^ -o $@

clean:
	rm -f *.elf

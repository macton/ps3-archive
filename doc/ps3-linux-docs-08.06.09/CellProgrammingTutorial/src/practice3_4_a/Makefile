CC         = gcc
CINCS      =
CLIBS      = -lspe2
CFLAGS     = -Wall

SPU_CC     = spu-gcc
SPU_CINCS  =
SPU_CLIBS  =
SPU_CFLAGS = -Wall

all: vec_grayscale_ppe.elf vec_grayscale_spe.elf

vec_grayscale_ppe.elf: main.o vec_grayscale_ppe.o
	$(CC) $(CFLAGS) $(CINCS) $(CLIBS) $^ -o $@

main.o: main.c
	$(CC) $(CFLAGS) $(CINCS) -c $^ -o $@

vec_grayscale_ppe.o: vec_grayscale_ppe.c
	$(CC) $(CFLAGS) $(CINCS) -c $^ -o $@

vec_grayscale_spe.elf: vec_grayscale_spe.c
	$(SPU_CC) $(SPU_CFLAGS) $(SPU_CINCS) $(SPU_CLIBS) $^ -o $@

clean:
	rm -f *.o *.elf

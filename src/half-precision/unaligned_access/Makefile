OBJDIR=cbe-spu-obj
SPUCC=spu-lv2-gcc
SPUCC_FLAGS=-Wall -DTEST_FLOAT_SP -I/usr/local/spusim/include -O3 -std=c99 -pedantic

test-cbe-spu.elf : $(OBJDIR) $(OBJDIR)/test.o $(OBJDIR)/unaligned_double.o $(OBJDIR)/vsl.o
	@echo "Linking test.elf"
	@$(SPUCC) $(OBJDIR)/test.o                \
                  $(OBJDIR)/unaligned_double.o    \
                  $(OBJDIR)/vsl.o    \
                  -o test.elf /usr/local/spusim/lib/sim.a

$(OBJDIR)/test.o : test.c
	@echo "Compiling test.c"
	@$(SPUCC) $(SPUCC_FLAGS) -c test.c -o $(OBJDIR)/test.o

$(OBJDIR)/vsl.o : vsl.c
	@echo "Compiling vsl.c"
	@$(SPUCC) $(SPUCC_FLAGS) -c vsl.c -o $(OBJDIR)/vsl.o

$(OBJDIR)/unaligned_double.o : unaligned_double.c
	@echo "Compiling unaligned_double.c"
	@$(SPUCC) $(SPUCC_FLAGS) -c unaligned_double.c -o $(OBJDIR)/unaligned_double.o

$(OBJDIR):
	@mkdir $(OBJDIR)

run:
	@spusim -run test.elf

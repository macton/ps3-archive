	.file	"test.c"
	.section	".text"
	.align 2
	.globl half_convert_alt
	.section	".opd","aw"
	.align 3
half_convert_alt:
	.quad	.half_convert_alt,.TOC.@tocbase,0
	.previous
	.size	half_convert_alt,24
	.type	.half_convert_alt,@function
	.globl	.half_convert_alt
.half_convert_alt:
	rldicl 5,3,41,56
	li 4,0
	srdi 0,3,16
	ori 11,4,32768
	addi 9,5,-112
	and 10,0,11
	extsw 11,9
	cmpwi 7,11,0
	rldicl 3,3,0,41
	ble- 7,.L17
	cmpwi 6,11,143
	beq- 6,.L18
	rlwinm 6,3,0,19,19
	cmpwi 1,6,0
	bne- 1,.L19
.L12:
	cmpwi 7,11,30
	ori 9,10,31744
	extsw 0,9
	bgt- 7,.L1
	slwi 0,11,10
	srawi 4,3,13
	or 11,0,10
	or 3,11,4
	rlwinm 0,3,0,0xffff
.L15:
	extsw 0,0
.L1:
	mr 3,0
	blr
.L19:
	addi 8,3,8192
	extsw 3,8
	rlwinm 7,3,0,8,8
	cmpwi 6,7,0
	beq- 6,.L12
	addi 12,11,1
	li 3,0
	extsw 11,12
	b .L12
.L18:
	cmpdi 7,3,0
	ori 11,10,31744
	extsw 0,11
	beq- 7,.L1
	srawi 5,3,13
	extsw 0,5
	or 4,10,0
	ori 9,4,31744
	cmpdi 0,0,0
	ori 10,9,1
	rlwinm 0,10,0,0xffff
	beq- 0,.L15
	rlwinm 0,9,0,0xffff
	extsw 0,0
	b .L1
.L17:
	cmpwi 0,11,-10
	li 0,0
	blt- 0,.L1
	oris 8,3,0x80
	subfic 3,11,1
	sraw 7,8,3
	extsw 3,7
	rlwinm 6,3,0,19,19
	cmpwi 1,6,0
	beq- 1,.L4
	addi 12,3,8192
	extsw 3,12
.L4:
	rldicl 9,3,51,48
	or 0,9,10
	mr 3,0
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	.half_convert_alt,.-.half_convert_alt
	.align 2
	.globl half_convert
	.section	".opd","aw"
	.align 3
half_convert:
	.quad	.half_convert,.TOC.@tocbase,0
	.previous
	.size	half_convert,24
	.type	.half_convert,@function
	.globl	.half_convert
.half_convert:
	mr 12,3
	li 5,4096
	rldicl 3,3,41,56
	and 4,12,5
	addi 7,3,-112
	li 6,0
	extsh 0,7
	sldi 11,4,1
	srdi 9,12,16
	rldicl 10,12,0,41
	ori 8,6,32768
	add 12,10,11
	and 6,9,8
	neg 5,0
	subfic 4,0,30
	sldi 3,7,10
	sradi 11,5,63
	sradi 8,4,63
	or 7,3,6
	srdi 10,12,13
	andc 0,11,8
	or 5,7,10
	andc 9,6,0
	and 4,5,0
	or 3,4,9
	rldicl 3,3,0,48
	blr
	.long 0
	.byte 0,0,0,0,0,0,0,0
	.size	.half_convert,.-.half_convert
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 3
.LC0:
	.string	"%f = %08x -> %04x\n"
	.section	".toc","aw"
.LC1:
	.tc .LC0[TC],.LC0
	.align 3
.LC2:
	.tc FD_3ff00000_0[TC],0x3ff0000000000000
	.section	".text"
	.align 2
	.globl main
	.section	".opd","aw"
	.align 3
main:
	.quad	.main,.TOC.@tocbase,0
	.previous
	.size	main,24
	.type	.main,@function
	.globl	.main
.main:
	lis 4,0x3ff0
	mflr 3
	stdu 1,-112(1)
	std 3,128(1)
	sldi 4,4,32
	ld 3,.LC1@toc(2)
	lis 5,0x3f80
	li 6,15360
	lfd 1,.LC2@toc(2)
	bl .printf
	nop
	ld 0,128(1)
	li 3,0
	mtlr 0
	addi 1,1,112
	blr
	.long 0
	.byte 0,0,0,1,128,0,0,0
	.size	.main,.-.main
	.ident	"GCC: (GNU) 3.4.1 (CELL 2.3, Aug 18 2005)"

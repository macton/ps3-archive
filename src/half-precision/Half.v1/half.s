	.file	"half.c"
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
	.section	.rodata.str1.8,"aMS",@progbits,1
	.align 3
.LC2:
	.string	"f16_norm_value: 0x%x\n"
	.align 3
.LC0:
	.string	"INF %d 0x%x 0x%x\n"
	.section	".toc","aw"
.LC1:
	.tc .LC0[TC],.LC0
.LC3:
	.tc .LC2[TC],.LC2
	.section	".text"
	.align 2
	.globl convert_half
	.section	".opd","aw"
	.align 3
convert_half:
	.quad	.convert_half,.TOC.@tocbase,0
	.previous
	.size	convert_half,24
	.type	.convert_half,@function
	.globl	.convert_half
.convert_half:
	stdu 1,-176(1)
	mflr 4
	std 25,120(1)
	rldicl 25,3,41,56
	std 4,192(1)
	addi 4,25,-127
	std 26,128(1)
	rldicl 26,3,0,41
	std 31,168(1)
	addi 31,25,-112
	neg 8,4
	std 24,112(1)
	std 29,152(1)
	xori 24,31,15
	subfic 10,24,0
	adde 24,10,24
	mr 29,3
	std 28,144(1)
	subfic 0,26,0
	adde 3,0,26
	std 27,136(1)
	li 6,0
	std 30,160(1)
	or 7,8,4
	and 12,3,24
	srdi 5,29,16
	sradi 0,7,63
	ori 10,6,32768
	extsh 28,31
	neg 11,3
	cmpdi 7,12,0
	and 27,5,10
	subfic 9,28,30
	andc 30,11,0
	mr 5,0
	sradi 25,9,63
	mr 6,30
	ori 24,27,31744
	bne- 7,.L29
.L22:
	li 9,4096
	ld 3,.LC3@toc(2)
	and 0,29,9
	sldi 11,0,1
	sldi 29,31,10
	add 4,26,11
	neg 28,28
	or 7,29,27
	sradi 12,28,63
	srdi 8,4,13
	andc 26,12,25
	or 25,7,8
	mr 4,25
	bl .printf
	nop
	and 10,26,25
	andc 5,24,26
	ld 25,120(1)
	addi 6,31,10
	ld 26,192(1)
	sradi 0,6,63
	mtlr 26
	or 3,10,5
	ld 26,128(1)
	and 11,0,27
	andc 9,3,0
	or 31,11,9
	and 28,30,24
	ld 24,112(1)
	andc 29,31,30
	ld 30,160(1)
	or 27,28,29
	ld 31,168(1)
	rldicl 3,27,0,48
	ld 28,144(1)
	ld 27,136(1)
	ld 29,152(1)
	addi 1,1,176
	blr
.L29:
	ld 3,.LC1@toc(2)
	bl .printf
	nop
	b .L22
	.long 0
	.byte 0,0,0,1,128,8,0,0
	.size	.convert_half,.-.convert_half
	.ident	"GCC: (GNU) 3.4.1 (CELL 2.3, Aug 18 2005)"

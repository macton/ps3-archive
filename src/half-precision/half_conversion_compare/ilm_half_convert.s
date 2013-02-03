	.section __TEXT,__text,regular,pure_instructions
	.section __TEXT,__picsymbolstub1,symbol_stubs,pure_instructions,32
	.machine ppc
	.text
	.align 2
	.globl _ilm_half_from_float
_ilm_half_from_float:
	rlwinm r9,r3,9,24,31
	rlwinm r2,r3,0,9,31
	addic. r11,r9,-112
	rlwinm r10,r3,16,16,16
	bgt+ cr0,L2
	cmpwi cr7,r11,-10
	bge+ cr7,L4
	li r3,0
	blr
L4:
	oris r0,r2,0x80
	subfic r2,r11,1
	sraw r2,r0,r2
	andi. r0,r2,4096
	beq- cr0,L7
	addi r2,r2,8192
L7:
	rlwinm r0,r2,19,16,31
	or r3,r0,r10
	blr
L2:
	cmpwi cr7,r11,143
	bne+ cr7,L9
	cmpwi cr7,r2,0
	beq- cr7,L19
	srawi r0,r2,13
	subfic r9,r0,0
	adde r2,r9,r0
	or r0,r10,r0
	or r2,r2,r0
	ori r2,r2,31744
	rlwinm r3,r2,0,0xffff
	blr
L9:
	andi. r0,r3,4096
	beq- cr0,L13
	addi r2,r2,8192
	andis. r0,r2,128
	beq- cr0,L13
	addi r11,r9,-111
	li r2,0
L13:
	cmpwi cr7,r11,30
	ble- cr7,L16
L19:
	ori r0,r10,31744
	rlwinm r3,r0,0,0xffff
	blr
L16:
	slwi r0,r11,10
	srawi r2,r2,13
	or r0,r0,r10
	or r0,r0,r2
	rlwinm r3,r0,0,0xffff
	blr
	.align 2
	.globl _ilm_half_to_float
_ilm_half_to_float:
	rlwinm r11,r3,22,27,31
	rlwinm r9,r3,0,22,31
	cmpwi cr7,r11,0
	srwi r2,r3,15
	bne- cr7,L22
	cmpwi cr7,r9,0
	bne- cr7,L35
	slwi r3,r2,31
	blr
L28:
	slwi r9,r9,1
	addi r3,r3,-1
	andi. r0,r9,1024
	beq+ cr0,L28
	rlwinm r9,r9,0,22,20
	addi r11,r3,1
	b L31
L22:
	cmpwi cr7,r11,31
	bne+ cr7,L31
	cmpwi cr7,r9,0
	bne- cr7,L33
	slwi r0,r2,31
	oris r3,r0,0x7f80
	blr
L33:
	slwi r0,r2,31
	slwi r2,r9,13
	or r0,r0,r2
	oris r3,r0,0x7f80
	blr
L31:
	addi r0,r11,112
	slwi r2,r2,31
	slwi r0,r0,23
	slwi r9,r9,13
	or r2,r2,r0
	or r3,r9,r2
	blr
L35:
	li r3,0
	b L28
	.subsections_via_symbols

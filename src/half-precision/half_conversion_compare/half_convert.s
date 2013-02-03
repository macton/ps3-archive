	.file	"half_convert.c"
	.text
	.p2align 4,,15
.globl half_from_float
	.type	half_from_float, @function
half_from_float:
.LFB18:
	movl	%edi, %r8d
	movl	%edi, %r10d
	movl	%edi, %r11d
	andl	$8388607, %r8d
	andl	$2139095040, %r10d
	andl	$4096, %r11d
	movl	%r8d, %esi
	leal	(%r8,%r11,2), %r11d
	leal	-2139095040(%r10), %eax
	leal	-939524096(%r10), %edx
	shrl	$13, %esi
	negl	%r8d
	negl	%esi
	orl	%eax, %r8d
	movl	$1, %ecx
	orl	%eax, %esi
	movl	%edx, %r9d
	orl	%r11d, %edx
	sarl	$31, %esi
	shrl	$13, %edx
	movl	%r11d, %eax
	andl	%esi, %edx
	notl	%esi
	shrl	$23, %r9d
	andl	$31745, %esi
	sarl	$31, %r8d
	orl	$8388608, %eax
	subl	%r9d, %ecx
	orl	%esi, %edx
	subl	$939524097, %r10d
	andl	%r8d, %edx
	shrl	%cl, %eax
	notl	%r8d
	sarl	$31, %r10d
	shrl	$13, %eax
	andl	$31744, %r8d
	andl	%r10d, %eax
	orl	%r8d, %edx
	notl	%r10d
	andl	$-2147483648, %edi
	andl	%r10d, %edx
	shrl	$16, %edi
	orl	%edx, %eax
	orl	%edi, %eax
	movzwl	%ax, %eax
	ret
.LFE18:
	.size	half_from_float, .-half_from_float
	.p2align 4,,15
.globl half_from_float_x2
	.type	half_from_float_x2, @function
half_from_float_x2:
.LFB20:
	movl	%edi, %r9d
	movq	%rbx, -48(%rsp)
.LCFI0:
	movl	%edi, %ebx
	andl	$8388607, %r9d
	andl	$2139095040, %ebx
	movq	%r15, -8(%rsp)
.LCFI1:
	movl	%r9d, %r8d
	movl	%edi, %r15d
	leal	-2139095040(%rbx), %edx
	movq	%r13, -24(%rsp)
.LCFI2:
	andl	$4096, %r15d
	movq	%rdi, %r13
	shrl	$13, %r8d
	shrq	$32, %r13
	leal	(%r9,%r15,2), %r15d
	negl	%r8d
	negl	%r9d
	movl	%r13d, %r11d
	orl	%edx, %r8d
	orl	%edx, %r9d
	movl	%r13d, %edx
	andl	$8388607, %r11d
	andl	$4096, %edx
	leal	-939524096(%rbx), %esi
	leal	(%r11,%rdx,2), %edx
	movq	%r12, -32(%rsp)
.LCFI3:
	movl	%r13d, %r12d
	andl	$2139095040, %r12d
	movl	%r11d, %r10d
	movq	%r14, -16(%rsp)
.LCFI4:
	movl	%edx, -52(%rsp)
	shrl	$13, %r10d
	leal	-2139095040(%r12), %edx
	movl	%esi, %ecx
	movl	$1, %r14d
	movq	%rbp, -40(%rsp)
.LCFI5:
	shrl	$23, %ecx
	negl	%r10d
	negl	%r11d
	movl	%r14d, %ebp
	orl	%edx, %r10d
	orl	%edx, %r11d
	subl	%ecx, %ebp
	movl	%r15d, %edx
	movl	%edi, %eax
	movl	%ebp, %ecx
	orl	$8388608, %edx
	subl	$939524097, %ebx
	shrl	%cl, %edx
	sarl	$31, %ebx
	leal	-939524096(%r12), %edi
	andl	$-2147483648, %eax
	subl	$939524097, %r12d
	shrl	$13, %edx
	sarl	$31, %r8d
	sarl	$31, %r9d
	sarl	$31, %r10d
	sarl	$31, %r11d
	sarl	$31, %r12d
	andl	%ebx, %edx
	shrl	$16, %eax
	orl	%r15d, %esi
	notl	%ebx
	shrl	$13, %esi
	andl	$-2147483648, %r13d
	movq	-40(%rsp), %rbp
	andl	%r8d, %esi
	notl	%r8d
	shrl	$16, %r13d
	andl	$31745, %r8d
	movq	-8(%rsp), %r15
	orl	%r8d, %esi
	andl	%r9d, %esi
	notl	%r9d
	andl	$31744, %r9d
	orl	%r9d, %esi
	andl	%ebx, %esi
	movq	-48(%rsp), %rbx
	orl	%esi, %edx
	movl	-52(%rsp), %esi
	orl	%edx, %eax
	movl	%edi, %edx
	orl	-52(%rsp), %edi
	shrl	$23, %edx
	movzwl	%ax, %eax
	subl	%edx, %r14d
	orl	$8388608, %esi
	movl	%r14d, %ecx
	movq	-16(%rsp), %r14
	shrl	$13, %edi
	shrl	%cl, %esi
	andl	%r10d, %edi
	notl	%r10d
	shrl	$13, %esi
	andl	$31745, %r10d
	andl	%r12d, %esi
	notl	%r12d
	orl	%r10d, %edi
	andl	%r11d, %edi
	notl	%r11d
	andl	$31744, %r11d
	orl	%r11d, %edi
	andl	%r12d, %edi
	movq	-32(%rsp), %r12
	orl	%edi, %esi
	orl	%esi, %r13d
	sall	$16, %r13d
	orl	%r13d, %eax
	movq	-24(%rsp), %r13
	ret
.LFE20:
	.size	half_from_float_x2, .-half_from_float_x2
	.p2align 4,,15
.globl half_to_float
	.type	half_to_float, @function
half_to_float:
.LFB19:
	movzwl	%di, %eax
	pushq	%rbx
.LCFI6:
	movl	%eax, %r8d
	movl	%eax, %edi
	andl	$32768, %eax
	andl	$1023, %r8d
	andl	$31744, %edi
	movl	$31744, -4(%rsp)
	bsrl	%r8d, %edx
	leal	-1(%rdi), %r9d
	subl	%r9d, -4(%rsp)
	movl	%r8d, %ebx
	xorl	$31, %edx
	movl	-4(%rsp), %r10d
	notl	-4(%rsp)
	leal	-8(%rdx), %ecx
	negl	%ebx
	movl	%r8d, %esi
	andl	%ebx, -4(%rsp)
	movl	$126, %edx
	addl	$114688, %edi
	sall	%cl, %esi
	subl	%ecx, %edx
	sarl	$31, %r10d
	movl	-4(%rsp), %ecx
	sall	$23, %edx
	andl	$8388607, %esi
	movl	%r9d, %r11d
	orl	%edx, %esi
	sall	$13, %edi
	movl	%r10d, %edx
	notl	%r10d
	andl	%ebx, %r11d
	andl	$2139095040, %edx
	andl	%r10d, %edi
	sall	$13, %r8d
	sarl	$31, %ecx
	sarl	$31, %r11d
	notl	%ebx
	orl	%edi, %edx
	andl	%ecx, %r8d
	andl	%r11d, %esi
	andl	%ebx, %r9d
	orl	%r8d, %edx
	notl	%r11d
	andl	%r11d, %edx
	sarl	$31, %r9d
	sall	$16, %eax
	popq	%rbx
	orl	%edx, %esi
	notl	%r9d
	andl	%r9d, %esi
	orl	%esi, %eax
	ret
.LFE19:
	.size	half_to_float, .-half_to_float
	.p2align 4,,15
.globl _half_to_float_x2
	.type	_half_to_float_x2, @function
_half_to_float_x2:
.LFB21:
	movzwl	%di, %edi
	pushq	%r12
.LCFI7:
	movl	$31744, %edx
	movl	%edi, %r8d
	movl	%edi, %esi
	andl	$32768, %edi
	andl	$1023, %r8d
	andl	$31744, %esi
	pushq	%rbp
.LCFI8:
	bsrl	%r8d, %eax
	leal	-1(%rsi), %r9d
	movl	%r8d, %r12d
	pushq	%rbx
.LCFI9:
	xorl	$31, %eax
	movl	%r8d, %ebx
	addl	$114688, %esi
	leal	-8(%rax), %ebp
	subl	%r9d, %edx
	movl	$126, %eax
	movl	%edx, %r10d
	negl	%ebx
	notl	%edx
	movl	%ebp, %ecx
	subl	%ebp, %eax
	sarl	$31, %r10d
	sall	%cl, %r12d
	sall	$23, %eax
	movl	%r9d, %r11d
	andl	$8388607, %r12d
	andl	%ebx, %edx
	sall	$13, %esi
	orl	%eax, %r12d
	movl	%r10d, %eax
	notl	%r10d
	andl	%ebx, %r11d
	andl	$2139095040, %eax
	andl	%r10d, %esi
	sall	$13, %r8d
	sarl	$31, %edx
	sarl	$31, %r11d
	notl	%ebx
	orl	%esi, %eax
	andl	%edx, %r8d
	andl	%r11d, %r12d
	andl	%ebx, %r9d
	orl	%r8d, %eax
	notl	%r11d
	sarl	$31, %r9d
	sall	$16, %edi
	andl	%r11d, %eax
	notl	%r9d
	orl	%eax, %r12d
	movl	%edi, %eax
	popq	%rbx
	andl	%r9d, %r12d
	popq	%rbp
	orl	%r12d, %eax
	popq	%r12
	ret
.LFE21:
	.size	_half_to_float_x2, .-_half_to_float_x2
	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1
.LSCIE1:
	.long	0x0
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -8
	.byte	0x10
	.byte	0xc
	.uleb128 0x7
	.uleb128 0x8
	.byte	0x90
	.uleb128 0x1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1
.LASFDE1:
	.long	.LASFDE1-.Lframe1
	.quad	.LFB18
	.quad	.LFE18-.LFB18
	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3
.LASFDE3:
	.long	.LASFDE3-.Lframe1
	.quad	.LFB20
	.quad	.LFE20-.LFB20
	.byte	0x4
	.long	.LCFI0-.LFB20
	.byte	0x83
	.uleb128 0x7
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0x8f
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI2-.LCFI1
	.byte	0x8d
	.uleb128 0x4
	.byte	0x4
	.long	.LCFI3-.LCFI2
	.byte	0x8c
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI4-.LCFI3
	.byte	0x8e
	.uleb128 0x3
	.byte	0x4
	.long	.LCFI5-.LCFI4
	.byte	0x86
	.uleb128 0x6
	.align 8
.LEFDE3:
.LSFDE5:
	.long	.LEFDE5-.LASFDE5
.LASFDE5:
	.long	.LASFDE5-.Lframe1
	.quad	.LFB19
	.quad	.LFE19-.LFB19
	.byte	0x4
	.long	.LCFI6-.LFB19
	.byte	0xe
	.uleb128 0x10
	.byte	0x83
	.uleb128 0x2
	.align 8
.LEFDE5:
.LSFDE7:
	.long	.LEFDE7-.LASFDE7
.LASFDE7:
	.long	.LASFDE7-.Lframe1
	.quad	.LFB21
	.quad	.LFE21-.LFB21
	.byte	0x4
	.long	.LCFI7-.LFB21
	.byte	0xe
	.uleb128 0x10
	.byte	0x4
	.long	.LCFI8-.LCFI7
	.byte	0xe
	.uleb128 0x18
	.byte	0x86
	.uleb128 0x3
	.byte	0x8c
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI9-.LCFI8
	.byte	0xe
	.uleb128 0x20
	.byte	0x83
	.uleb128 0x4
	.align 8
.LEFDE7:
	.ident	"GCC: (GNU) 4.0.2 20051125 (Red Hat 4.0.2-8)"
	.section	.note.GNU-stack,"",@progbits

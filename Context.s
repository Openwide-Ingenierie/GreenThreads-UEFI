	.intel_syntax
	.text
	.set OFS, 8
	.globl SwitchContext
	
GetContext:
	# Basic registers
	mov [(%rcx+OFS*0)], %rax
	mov [(%rcx+OFS*1)], %rbx
	# mov [(%rcx+OFS*2)], %rcx
	mov [(%rcx+OFS*3)], %rdx
	mov [(%rcx+OFS*4)], %rbp
	mov [(%rcx+OFS*5)], %rsi
	mov [(%rcx+OFS*6)], %rdi
	# mov [(%rcx+OFS*7)], %rsp
	
	# R registers
	mov [(%rcx+OFS*8)], %r8
	mov [(%rcx+OFS*9)], %r9
	mov [(%rcx+OFS*10)], %r10
	mov [(%rcx+OFS*11)], %r11
	mov [(%rcx+OFS*12)], %r12
	mov [(%rcx+OFS*13)], %r13
	mov [(%rcx+OFS*14)], %r14
	mov [(%rcx+OFS*15)], %r15
	
	ret

SetContext:
	# Basic registers
	mov %rax, [(%rcx+OFS*0)]
	mov %rbx, [(%rcx+OFS*1)]
	# mov %rcx, [(%rcx+OFS*2)]
	mov %rdx, [(%rcx+OFS*3)]
	mov %rbp, [(%rcx+OFS*4)]
	mov %rsi, [(%rcx+OFS*5)]
	mov %rdi, [(%rcx+OFS*6)]
	# mov %rsp, [(%rcx+OFS*7)]

	# R registers
	mov %r8,  [(%rcx+OFS*8)]
	mov %r9,  [(%rcx+OFS*9)]
	mov %r10, [(%rcx+OFS*10)]
	mov %r11, [(%rcx+OFS*11)]
	mov %r12, [(%rcx+OFS*12)]
	mov %r13, [(%rcx+OFS*13)]
	mov %r14, [(%rcx+OFS*14)]
	mov %r15, [(%rcx+OFS*15)]
	ret
	
	# Routine changing the context from Old (%rcx) to New (%rdx)
SwitchContext:
	call GetContext
	mov [(%rcx+OFS*7)], %rsp
	mov %rcx, %rdx
	call SetContext
	mov %rsp, [(%rcx+OFS*7)]
	ret

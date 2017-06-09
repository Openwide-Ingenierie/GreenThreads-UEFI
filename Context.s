	.intel_syntax
	.text
	.set OFS, 8
	.globl GetContext
	.globl SetContext
	.globl SwitchContext
	//.globl ChangeContext
	
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

	/*# Mmx registers
	mov [(%rcx+OFS*16)], %mmx0
	mov [(%rcx+OFS*17)], %mmx1
	mov [(%rcx+OFS*18)], %mmx2
	mov [(%rcx+OFS*19)], %mmx3
	mov [(%rcx+OFS*20)], %mmx4
	mov [(%rcx+OFS*21)], %mmx5
	mov [(%rcx+OFS*22)], %mmx6
	mov [(%rcx+OFS*23)], %mmx7
	mov [(%rcx+OFS*24)], %rflags
	mov [(%rcx+OFS*26)], %rsp*/
	
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
	
/*ChangeContext:
	xor %rdx, %rdx
	mov %rcx, [Count+%rip] # rcx <- Count
	mov %rax, [RunningId+%rip]
	inc %rax  # rax <- RunningId+1
	idiv %rcx # rdx = NextRunning <=> (rdx:rax) % rcx
	mov %rax, [RunningId+%rip]
	cmp %rdx, %rax
	je nochange
	# NextRunning != RunningId, let's check if it's ready
	# RAX = RunningId, RDX = NextRunning, RCX = Count
check:
	cmp %rdx, %rax
	je endloop
	# Test whether the context is runnable or not
	imul %r8, %rdx, 8 # sizeof(void*) = 8
	lea %r9, [Contexts + %rip]
	mov %r8, [%r8 + %r9] # R8 = Contexts[NextRunning]
	mov %r9, [(%r8+OFS*27)] # R9 = R8->Stat
	cmp %r9, 0 # R9 == THREAD_RUNNABLE
	je endloop
	
	inc %rdx
	cmp %rdx, %rcx # If NextRunning == Count, then NextRunning = 0
	jne check
	xor %rdx, %rdx
	jmp check
endloop:	
	# We have to change the context, so first, backup context
	mov %rax, [RunningId+%rip]
	imul %rcx, %rax, 8 # sizeof(void*) = 8
	lea %rax, [Contexts + %rip]
	mov %rax, [%rax + %rcx]
	call GetContext
	# Save the stack pointer
	mov [(%rax+OFS*7)], %rsp
	
	# RunningId = NextRunning
	mov [RunningId+%rip], %rdx

	# Set next context
	imul %rcx, %rdx, 8 # sizeof(void*) = 8
	lea %rax, [Contexts + %rip]
	mov %rax, [%rax + %rcx]
	call SetContext
	mov %rsp, [(%rax+OFS*7)]
nochange:
	ret
*/
	# Routine changing the context from Old (%rcx) to New (%rdx)
SwitchContext:
	call GetContext
	mov [(%rcx+OFS*7)], %rsp
	mov %rcx, %rdx
	call SetContext
	mov %rsp, [(%rcx+OFS*7)]
	ret

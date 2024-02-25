/* hello.S */

.section .rodata

sInputPromptMsg: .string "Enter 10 integer values: "
sInputFmt:       .string "%ld"
sResultMsg:      .string "Max is %ld\n"

.section .bss

	.align 16
dataValues: .space (10 * 4)

.section .text

	.globl main
main:
	pushq %r12
	pushq %r13
	subq $8, %rsp

	movq $sInputPromptMsg, %rdi
	call printf

	/* read data values */
	movq $dataValues, %r12        /* %r12 stores base addr of array */
	movl $0, %r13d                /* %r13 is index into array */

	jmp .LinputLoopCond
.LinputLoopTop:
	movq $sInputFmt, %rdi
	leaq (%r12,%r13,4), %rsi
	call scanf
	incl %r13d

.LinputLoopCond:
	cmpl $10, %r13d
	jl .LinputLoopTop

	/* find the maximum value */
	movl (%r12), %r10d            /* initially assume first element is max */
	movl $0, %r13d                /* %r13 is index into array */
	jmp .LfindMaxLoopCond

.LfindMaxLoopTop:
	cmpl %r10d, (%r12,%r13,4)     /* compare element to current max */
	jbe .LfindMaxLoopIncrIndex     /* if cur elt < max, no update */
	movl (%r12,%r13,4), %r10d     /* update current elt as max */

.LfindMaxLoopIncrIndex:
	incl %r13d                    /* increment index */

.LfindMaxLoopCond:
	cmpl $10, %r13d
	jl .LfindMaxLoopTop

	movq $sResultMsg, %rdi
	movl %r10d, %esi
	call printf

	popq %r13
	popq %r12
	addq $8, %rsp
	movl $0, %eax                 /* return 0 from main */
	ret

/*
vim:ft=gas:
*/

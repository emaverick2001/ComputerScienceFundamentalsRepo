/* hello.S */

.section .rodata

sHelloMsg:  .string "Hello, world!\n"
sPromptMsg: .string "Enter an integer: "
sInputFmt:  .string "%ld"
sResultMsg: .string "You entered: %ld\n"

.section .bss

num: .space 8

.section .text

	.globl main
main:
	subq $8, %rsp

	movq $sHelloMsg, %rdi
	call printf

	movq $sPromptMsg, %rdi
	call printf

	movq $sInputFmt, %rdi
	movq $num, %rsi
	call scanf

	movq $sResultMsg, %rdi
	movq num, %rsi
	call printf

	addq $8, %rsp
	movl $0, %eax                 /* return 0 from main */
	ret

/*
vim:ft=gas:
*/

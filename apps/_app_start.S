#include "kernel/asmdefs.h"

	.syntax unified
	
        .section .stack
	.align	3

	.equ	Stack_Size, 0x1000
	.globl	__StackTop
	.globl	__StackLimit
__StackLimit:
	.space	Stack_Size
	.size	__StackLimit, . - __StackLimit
__StackTop:
	.size	__StackTop, . - __StackTop
	
	.text
FUNC(_app_start)
	b	_start
ENDFUNC(_app_start)

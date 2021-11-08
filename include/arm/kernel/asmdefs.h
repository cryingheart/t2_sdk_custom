#pragma once

#ifdef ROM_BUILD
#define FUNC(_name) \
        .balign 4; \
        .globl  _name; \
        .type   _name, %function; \
        _name: \
        .reloc .,R_ARM_THM_PC12,.;\
        .inst 0xf99ff000
#else
#define FUNC(_name) \
        .balign 2; \
        .globl  _name; \
        .type   _name, %function; \
        _name:
#endif

#define ENDFUNC(_name) \
        .pool; \
        .size   _name, . - _name

#define IDLE .inst 0xbfe0

#ifdef __ASSEMBLER__
    .altmacro
    .macro ld32 reg, name
        movw    \reg, #:lower16:\name
        movt    \reg, #:upper16:\name
    .endm

    .macro prof4 value,opc1,crn,opc2,crm,reg
    .if     \value
    .error "Profiler immediate value too large."
    .endif
    mcr p7, \opc1, \reg, c\crn, c\crm, \opc2
    .endm

    .macro prof3 value, crn,opc2,crm,reg
    prof4 %value/8, %value%8, \crn, \opc2, \crm, \reg
    .endm

    .macro prof2 value, opc2,crm,reg
    prof3 %value/16, %value%16, \opc2, \crm, \reg
    .endm

    .macro prof1 value, crm, reg
    prof2 %value/8, %value%8, \crm, \reg
    .endm

    .macro prof value, reg
    prof1 %value/16, %value%16, \reg
    .endm
#endif

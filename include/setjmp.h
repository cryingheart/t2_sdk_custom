#pragma once
#include <bits/noreturn.h>

typedef struct { unsigned int state[10]; } jmp_buf[1];

int  setjmp(jmp_buf jb);
void longjmp(jmp_buf jb, int ret) __noreturn;

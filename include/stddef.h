#pragma once

#include <bits/null.h>
#include <bits/size_t.h>
#include <bits/wchar_t.h>

typedef __PTRDIFF_TYPE__ ptrdiff_t;

typedef unsigned long long max_align_t;

#define offsetof(type, member) __builtin_offsetof(type, member)
#define __offsetof offsetof

typedef int off_t;

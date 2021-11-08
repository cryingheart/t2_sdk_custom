#pragma once

#define DEBUG_PACKET

#ifdef DEBUG_PACKET
#include <assert.h>
#include <stdint.h>
#define MAGIC_PACKET 0xc0de1111
#define MAGIC_FRAG   0xc0de2222
#define MAGIC_DATA   0xc0de3333
#define DECLARE_MAGIC(_x) uint32_t _x
#define INIT_PACKET(_expr)   do { (_expr)->p_magic = MAGIC_PACKET; } while (0)
#define INIT_FRAG(_expr)     do { (_expr)->pf_magic = MAGIC_FRAG; } while (0)
#define INIT_DATA(_expr)     do { (_expr)->db_magic = MAGIC_DATA; } while (0)
#define ASSERT_PACKET(_expr) assert((_expr)->p_magic == MAGIC_PACKET)
#define ASSERT_FRAG(_expr)   assert((_expr)->pf_magic == MAGIC_FRAG)
#define ASSERT_DATA(_expr)   assert((_expr)->db_magic == MAGIC_DATA)
#else
#define DECLARE_MAGIC(_x)
#define INIT_PACKET(_expr)
#define INIT_FRAG(_expr)
#define INIT_DATA(_expr)
#define ASSERT_PACKET(_expr)
#define ASSERT_FRAG(_expr)
#define ASSERT_DATA(_expr)
#endif

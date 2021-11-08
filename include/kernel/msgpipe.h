#pragma once
#include <stdint.h>

#define MSGPIPE_MAX 16

typedef uint32_t cop_msg_t;


struct msgpipe {
    cop_msg_t pipe[MSGPIPE_MAX];
    uint32_t  w_ptr;
    uint32_t  r_ptr;
};

static inline unsigned
cop_msg_id(cop_msg_t m)
{
    return m >> 24;
}

static inline void *
cop_msg_arg(cop_msg_t m)
{
    return (void *)(m & 0xffffff);
}

void msgpipe_init(struct msgpipe *p);
int msgpipe_put(struct msgpipe *p, unsigned id, void *arg);
int msgpipe_avail(struct msgpipe *p);
cop_msg_t msgpipe_get(struct msgpipe *p);

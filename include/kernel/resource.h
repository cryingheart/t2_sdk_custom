#pragma once
#include <stdint.h>
#include <kernel/atomic.h>

struct resource {
    atomic_t rs_refs;
    void   (*rs_ini)(struct resource*);
    void   (*rs_fini)(struct resource*);
};

void resource_init(struct resource *r,
                   void (*ini)(struct resource*),
                   void (*fini)(struct resource*));
void resource_request(struct resource *r, uint32_t user);
void resource_release(struct resource *r, uint32_t user);


/* reference counted interface to single resource user */
struct resource_ref {
    struct resource *rr_res;
    uint32_t         rr_user;
    atomic_t         rr_count;
};

void resource_ref_init(struct resource_ref *rr,
                       struct resource *res, uint32_t user);

void resource_ref_release(struct resource_ref *rr);

void resource_ref_request(struct resource_ref *rr);

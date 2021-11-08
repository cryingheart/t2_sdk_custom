#pragma once
#include <stdint.h>

#define HW_MMU_TLB_I               0
#define HW_MMU_TLB_D               1

struct hw_mmu {
    uint8_t                        enable;
#define MMU_ENABLE_I               0x1
#define MMU_ENABLE_I_WALK          0x2
#define MMU_ENABLE_D               0x4
#define MMU_ENABLE_D_WALK          0x8
    uint8_t                        skew;
    uint8_t                        status;
#define MMU_STATUS_I               0x01
#define MMU_STATUS_D               0x02
    uint32_t                       baseaddr;
    uint32_t                       evict_ppa;
    struct hw_mmu_unit {
        uint32_t                   fault_addr;
        uint16_t                   fault_pte;
        uint8_t                    lru;
        struct hw_mmu_unit_tlb {
            uint32_t               vpa;
            uint32_t               ppa;
#define MMU_PPA_PPA_MASK           0x00ffffff
#define MMU_PPA_VPS_MASK           0x0f000000
#define MMU_PPA_EN                 0x10000000
        } tlb[16];
    } unit[2];
};

#pragma once
#include <stdint.h>

#define HW_RESET_UNIT_WL_POR       2
#define HW_RESET_UNIT_WL_RST       5

#define HW_SM_WL_WAVE              9
#define HW_SM_WL_TIMEOUT           8

struct hw_pmu {
    uint8_t                        reset_reason;
#define PMU_RESET_REASON_POR       0x1
#define PMU_RESET_REASON_WAKEUP    0x2
#define PMU_RESET_REASON_WDOG      0x4
#define PMU_RESET_REASON_CPU       0x8
#define PMU_RESET_REASON_SPI       0x10
#define PMU_RESET_REASON_JTAG      0x20
    struct hw_pmu_reset_unit {
        uint16_t                   device[2];
#define PMU_DEVICE_MAC             0x0001
#define PMU_DEVICE_MAC_REG         0x0002
#define PMU_DEVICE_CPU_MASK        0x000c
#define PMU_DEVICE_COP_MASK        0x0070
#define PMU_DEVICE_CIPHER          0x0080
#define PMU_DEVICE_PHY             0x0100
#define PMU_DEVICE_PHY_REG         0x0200
#define PMU_DEVICE_AFE_REG         0x0400
#define PMU_DEVICE_SDIO            0x0800
#define PMU_DEVICE_I2C             0x1000
#define PMU_DEVICE_FREQ            0x2000
#define PMU_DEVICE_SPI_SLV         0x4000
    } reset_unit;
    uint8_t                        clock_sel;
#define PMU_CLOCK_SEL_XO           0x01
#define PMU_CLOCK_SEL_XTAL         0x02
#define PMU_CLOCK_SEL_LFC          0x04
#define PMU_CLOCK_SEL_HKADC        0x08
#define PMU_CLOCK_SEL_RCT          0x10
    struct hw_pmu_ram {
        uint8_t                    pwd_n;
    } ram;
    struct hw_pmu_always_on_vset {
        uint8_t                    normal;
        uint8_t                    powersave;
    } always_on_vset;
    uint8_t                        enter_sleep;
    uint8_t                        fail_sleep;
    uint32_t                       reserved[2];
    struct hw_pmu_sm {
        uint32_t                   sleep_time;
        uint32_t                   sleep_actual;
        uint8_t                    state_normal;
        uint8_t                    state_sleep;
        uint8_t                    state_last;
        uint16_t                   wave[16];
#define PMU_WAVE_XTAL_EN           0x01
#define PMU_WAVE_PD_BUCK           0x02
#define PMU_WAVE_PD_MDIG           0x04
#define PMU_WAVE_VSET              0x08
#define PMU_WAVE_ISOLATE           0x10
#define PMU_WAVE_UNUSED_5          0x20
#define PMU_WAVE_RESET             0x40
#define PMU_WAVE_UNUSED_7          0x80
#define PMU_WAVE_MASK              0x100
        uint8_t                    timeout[16];
#define PMU_TIMEOUT_NOTIMEOUT      0xff
    } sm;
    struct hw_pmu_xtal {
        uint8_t                    override;
#define PMU_OVERRIDE_DISABLE_SM    0x01
#define PMU_OVERRIDE_EN_CHIRP_OSC  0x02
#define PMU_OVERRIDE_PE_BOOST      0x04
#define PMU_OVERRIDE_PE_CORE_BIAS  0x08
#define PMU_OVERRIDE_CT_SEL        0x10
#define PMU_OVERRIDE_PE_CORE       0x20
#define PMU_OVERRIDE_PE_BUFF       0x40
        uint8_t                    cb;
        uint8_t                    rt;
        uint8_t                    tb;
        uint8_t                    ct[2];
        uint8_t                    count[4];
        uint8_t                    en_test_buffer;
    } xtal;
    struct hw_pmu_buck {
        uint8_t                    ldo_mode[2];
        uint8_t                    pwm_fc;
        uint8_t                    tm1;
        uint8_t                    tm2;
        uint8_t                    pd_div;
        uint8_t                    fosc_l;
        uint8_t                    en_oc;
        uint8_t                    vset[2];
        uint8_t                    vtrim;
        uint8_t                    pd_buck_1;
        uint16_t                   pd;
#define PMU_PD_RAM_0               0x0001
#define PMU_PD_RAM_1               0x0002
#define PMU_PD_RAM_2               0x0004
#define PMU_PD_RAM_3               0x0008
#define PMU_PD_RAM_4               0x0010
#define PMU_PD_RAM_5               0x0020
#define PMU_PD_RAM_6               0x0040
#define PMU_PD_RAM_7               0x0080
#define PMU_PD_RC32K               0x0100
#define PMU_PD_XO32K               0x0200
#define PMU_PD_EFSW                0x0400
        uint8_t                    rc32k_rbit;
        uint8_t                    vset_ldo_mdig;
        uint8_t                    vset_ldo_vio;
    } buck;
    struct hw_pmu_id {
        uint32_t                   chipid;
        uint8_t                    maskid;
        uint8_t                    packid;
        uint8_t                    packid_en;
    } id;
    uint8_t                        rst_mask[3];
#define PMU_RST_MASK_POR_ONCE      0x1
#define PMU_RST_MASK_POR           0x2
#define PMU_RST_MASK_RESET         0x4
};

#pragma once
#include <stdint.h>

struct hw_dpll {
    uint8_t                        reset;
    uint8_t                        enable;
#define DPLL_ENABLE_DIVIDER_RESET  0x01
#define DPLL_ENABLE_FRAC_N_ZERO    0x02
#define DPLL_ENABLE_LOOP_FILTER    0x04
#define DPLL_ENABLE_PPS            0x08
#define DPLL_ENABLE_PHASE_MODULATION 0x10
#define DPLL_ENABLE_TRAINER_MASK   0x60
    struct hw_dpll_tdc {
        struct hw_dpll_tdc_lut {
            uint16_t               fine[64];
            uint8_t                sign;
            uint8_t                use_pfd;
            uint8_t                offset;
        } lut;
    } tdc;
    struct hw_dpll_frac_n {
        uint8_t                    c;
        uint16_t                   f;
        uint16_t                   frac;
        uint16_t                   k;
        uint16_t                   offset;
    } frac_n;
    struct hw_dpll_loop_filter {
        uint8_t                    scaler_a;
        uint8_t                    scaler_b;
        uint8_t                    scaler_c;
        uint8_t                    freeze;
        uint16_t                   avg;
    } loop_filter;
    struct hw_dpll_lock {
        uint8_t                    src;
        uint8_t                    rng;
        uint8_t                    locked;
    } lock;
    struct hw_dpll_pps {
        uint16_t                   lms;
        uint8_t                    kp;
        uint8_t                    lms_gain;
        uint8_t                    delay;
    } pps;
    struct hw_dpll_psc {
        uint8_t                    n_i_delay;
        uint8_t                    n_o_delay;
        uint8_t                    p_delay;
        uint8_t                    f_delay;
        uint16_t                   f_threshold;
    } psc;
    struct hw_dpll_dco_gain {
        uint8_t                    override;
#define DPLL_OVERRIDE_FREQ_SEL     0x01
#define DPLL_OVERRIDE_BIAS         0x02
#define DPLL_OVERRIDE_USE_QUAD     0x04
#define DPLL_OVERRIDE_NEG_EDGE     0x08
        uint8_t                    pwl_span;
        uint16_t                   freq_override;
        struct hw_dpll_dco_gain_gain {
            uint16_t               trk;
            uint16_t               dac;
            uint32_t               pwl;
        } gain[9];
        uint8_t                    trk_scale;
        uint8_t                    dac_scale;
        struct hw_dpll_dco_gain_readback {
            uint8_t                acq;
            uint8_t                trk;
            uint8_t                dac;
        } readback;
    } dco_gain;
    struct hw_dpll_bkg {
        uint8_t                    start;
        uint8_t                    sel;
#define DPLL_SEL_PVT               0x1
#define DPLL_SEL_ACQ               0x2
#define DPLL_SEL_TRK               0x4
#define DPLL_SEL_DAC               0x8
        uint8_t                    fcw_override;
        uint16_t                   timeout;
        uint16_t                   count;
        uint8_t                    pvt;
        uint8_t                    acq;
        uint8_t                    trk;
        uint8_t                    dac;
        uint8_t                    busy;
    } bkg;
    struct hw_dpll_rms {
        uint8_t                    enable;
        uint8_t                    count;
        uint8_t                    lock_range;
        uint32_t                   value;
        uint8_t                    lock;
    } rms;
    struct hw_dpll_dc {
        uint16_t                   coeff;
    } dc[2];
    struct hw_dpll_pull {
        uint8_t                    phase;
        uint8_t                    env;
        uint8_t                    gain;
    } pull;
    struct hw_dpll_frac_d {
        uint8_t                    acq;
        uint8_t                    trk;
        uint8_t                    dac;
    } frac_d;
    uint8_t                        trainer_rate;
    uint16_t                       trainer_const;
    uint8_t                        mmd_override;
    uint16_t                       trace_sel;
#define DPLL_TRACE_SEL_TDC_OUT     0x01
#define DPLL_TRACE_SEL_TDC         0x02
#define DPLL_TRACE_SEL_LOOP        0x04
#define DPLL_TRACE_SEL_ERR         0x08
#define DPLL_TRACE_SEL_UNUSED_0    0x10
#define DPLL_TRACE_SEL_UNUSED_1    0x20
#define DPLL_TRACE_SEL_UNUSED_2    0x40
#define DPLL_TRACE_SEL_UNUSED_3    0x80
#define DPLL_TRACE_SEL_TRK_DAC     0x100
#define DPLL_TRACE_SEL_PVT_ACQ     0x200
    uint8_t                        trace_mode;
#define DPLL_TRACE_MODE_MODE_8     0x01
#define DPLL_TRACE_MODE_STORE_IDLE 0x02
    struct hw_dpll_syn {
        struct hw_dpll_syn_dco {
            uint8_t                bias;
            uint8_t                dco_testbuff_biasadj;
            struct hw_dpll_syn_dco_delay {
                uint8_t            delay;
                uint8_t            vdd1_en;
                uint8_t            vdd2_en;
                uint8_t            fc_en;
                uint8_t            sw1k_en;
                uint8_t            sw4k_en;
                uint8_t            sw16k_en;
                uint8_t            delay_in_sel;
                uint8_t            dff_clk_sel;
            } delay;
            uint8_t                misc[2];
            uint8_t                ldo;
#define DPLL_LDO_PD                0x01
#define DPLL_LDO_BP                0x02
            uint16_t               pe;            /* set-clear */
#define DPLL_PE_BIASDCO            0x0001
#define DPLL_PE_DCO_TESTBUFF       0x0002
#define DPLL_PE_DCOBUFF            0x0004
#define DPLL_PE_DCO_LO_2G          0x0008
#define DPLL_PE_DAC_BIAS_EN        0x0010
#define DPLL_PE_DCO_BIAS_EN        0x0020
#define DPLL_PE_DCO_BIAS_FCEN      0x0040
            uint8_t                rcw_acq;
            uint8_t                rcw_trk;
            uint8_t                pe_pre_driver;
            uint8_t                dac_step;
        } dco;
        struct hw_dpll_syn_mmd {
            struct hw_dpll_syn_mmd_ldo {
                uint8_t            ctrl;
#define DPLL_CTRL_PD               0x01
#define DPLL_CTRL_BP               0x02
                uint8_t            vset;
                uint8_t            en_istdby;
            } ldo;
            uint8_t                pe;
        } mmd;
        struct hw_dpll_syn_tdc {
            uint16_t               en;            /* set-clear */
#define DPLL_EN_EN_PFD             0x0001
#define DPLL_EN_FTDC_EN_ARB        0x0002
#define DPLL_EN_FTDC_EN_IN         0x0004
            uint8_t                ctrl_del_d;
            uint8_t                ctrl_del_r;
            uint8_t                ftdc_ctrl_f;
            uint8_t                ftdc_ctrl_s;
            uint8_t                ftdc_ldo_en_istdby;
            struct hw_dpll_syn_tdc_ldo {
                uint8_t            ctrl;
#define DPLL_CTRL_PD               0x01
#define DPLL_CTRL_BP               0x02
                uint8_t            vset;
            } ldo;
        } tdc;
        struct hw_dpll_syn_txdiv {
            uint8_t                txdiv1p25_init;
            uint8_t                ctrl;
#define DPLL_CTRL_TXDIV2P5_OUTBUFF_PON 0x01
#define DPLL_CTRL_TXDIV2P5_DIV2_PON 0x02
#define DPLL_CTRL_TXDIV1P25_EN     0x04
#define DPLL_CTRL_TXDIV2_EN        0x08
        } txdiv;
        struct hw_dpll_syn_txilo {
            struct hw_dpll_syn_txilo_ldo {
                uint8_t            ctrl;
#define DPLL_CTRL_PD               0x01
#define DPLL_CTRL_BP               0x02
                uint8_t            vset;
            } ldo;
            uint8_t                cm;
            uint8_t                pon;
            uint8_t                buf_pon;
        } txilo;
    } syn;
    uint8_t                        fcw_clock_neg_edge;
    uint32_t                       rev[5];
};

#pragma once
#include <stdint.h>

struct hw_afe {
    struct hw_afe_rfa {
        struct hw_afe_rfa_rx {
            uint8_t                misc[4];
            struct hw_afe_rfa_rx_lna1 {
                uint8_t            bias_ctrl;
                struct hw_afe_rfa_rx_lna1_cap {
                    uint8_t        bank;
                } cap;
                struct hw_afe_rfa_rx_lna1_ldo {
                    uint8_t        ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                    uint8_t        vset;
                } ldo;
                uint8_t            pon;
#define AFE_PON_PON                0x01
#define AFE_PON_PDET               0x02
#define AFE_PON_BUFF_CAL           0x04
#define AFE_PON_TEST_BUFF          0x08
            } lna1;
            struct hw_afe_rfa_rx_lna2 {
                struct hw_afe_rfa_rx_lna2_cap {
                    uint8_t        band;
                    uint8_t        coarse;
                    uint8_t        fine;
                } cap;
                uint8_t            cs[5];
                uint8_t            div;
                uint8_t            gm_bias;
                struct hw_afe_rfa_rx_lna2_ldo {
                    uint8_t        ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                    uint8_t        vset;
                } ldo;
                uint8_t            pon;
#define AFE_PON_PON                0x01
#define AFE_PON_PDET               0x02
#define AFE_PON_BUFF_CAL           0x04
#define AFE_PON_TEST_BUFF          0x08
#define AFE_PON_COMP               0x10
#define AFE_PON_DIV                0x20
#define AFE_PON_PON_BLE            0x40
                uint8_t            cc;
                uint8_t            vb_subvt_ctrl;
            } lna2;
            struct hw_afe_rfa_rx_lna3 {
                struct hw_afe_rfa_rx_lna3_cap {
                    uint8_t        band;
                    uint8_t        coarse;
                    uint8_t        fine;
                } cap;
                uint8_t            cs[5];
                uint8_t            div;
                uint8_t            gm_bias;
                struct hw_afe_rfa_rx_lna3_ldo {
                    uint8_t        ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                    uint8_t        vset;
                } ldo;
                uint8_t            pon;
#define AFE_PON_PON                0x01
#define AFE_PON_PDET               0x02
#define AFE_PON_BUFF_CAL           0x04
#define AFE_PON_TEST_BUFF          0x08
#define AFE_PON_COMP               0x10
#define AFE_PON_DIV                0x20
                uint8_t            cc;
                uint8_t            vb_subvt_ctrl;
            } lna3;
            struct hw_afe_rfa_rx_rx_lna {
                struct hw_afe_rfa_rx_rx_lna_iptx {
                    uint8_t        ctrl;
                    uint8_t        ibg_ctrl;
                    uint8_t        ipt_ctrl;
                } iptx;
                uint8_t            pon_bias;
                uint8_t            test_out_1_sw_en;
            } rx_lna;
            uint8_t                ck_160m_tx;
            struct hw_afe_rfa_rx_div_mix {
                uint8_t            en_ck_640m;
                uint8_t            sw_q;
                uint8_t            sw_i;
                uint8_t            en;
                uint8_t            cb_mix;
                uint8_t            cb_buff;
            } div_mix;
        } rx;
        struct hw_afe_rfa_pmix {
            struct hw_afe_rfa_pmix_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
            } ldo;
            uint8_t                pon;
            uint8_t                en_in_test;
            uint8_t                rx_ptia_en_test;
            uint8_t                ct;
        } pmix;
        struct hw_afe_rfa_ptia {
            uint8_t                lg;
        } ptia;
        struct hw_afe_rfa_lobuff {
            uint8_t                cc;
            uint8_t                mode;
        } lobuff;
        struct hw_afe_rfa_ilo {
            struct hw_afe_rfa_ilo_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
            } ldo;
            struct hw_afe_rfa_ilo_delay {
                uint16_t           coarse;
                uint8_t            fine;
            } delay;
            uint8_t                pvt;
            uint8_t                acq;
            uint8_t                cm[2];
            uint8_t                track;
            uint8_t                band;
            uint8_t                pon;
            uint8_t                en_test_out;
            uint8_t                cal_en;
            uint8_t                ilo_pe;
#define AFE_ILO_PE_BLE             0x01
#define AFE_ILO_PE_BLE_IN          0x02
#define AFE_ILO_PE_BLE_PHASE       0x04
#define AFE_ILO_PE_MIXER           0x08
#define AFE_ILO_PE_WIFI            0x10
#define AFE_ILO_PE_WIFI_IN         0x20
#define AFE_ILO_PE_WIFI_PHASE      0x40
#define AFE_ILO_PE_LO_PATH         0x80
            struct hw_afe_rfa_ilo_lbk {
                uint8_t            en;
                uint8_t            in_en;
                uint8_t            attn;
            } lbk;
        } ilo;
        struct hw_afe_rfa_padc {
            uint8_t                pd;
            uint8_t                en_chg;
            uint8_t                mode_std;
            uint16_t               dcoc;
            uint8_t                res_sel;
            uint8_t                buf;
            uint8_t                opamp;
            uint8_t                vcm;
        } padc;
        struct hw_afe_rfa_rxtdc {
            uint8_t                en_in;
            uint8_t                en_arb;
            uint8_t                ftdc_scale;
            uint8_t                ftdc_smpl_edge;
            uint8_t                en_digital_logic;
            uint16_t               ctdc_scale;
            uint32_t               offset;
            uint8_t                corr[6];
            uint8_t                en_40_or_80m;
            uint16_t               clock;
#define AFE_CLOCK_EN_640M_CLK      0x0001
#define AFE_CLOCK_EN_640M_CLK_TEST 0x0002
#define AFE_CLOCK_EN_CK_640M_DIG   0x0004
#define AFE_CLOCK_EN_CK_160M_DIG   0x0008
#define AFE_CLOCK_EN_CK_160M_DPA   0x0010
#define AFE_CLOCK_EN_CK_640M_DPA   0x0020
#define AFE_CLOCK_EN_CK_160M_TX_TDC 0x0040
#define AFE_CLOCK_EN_CK_160M_TX_TDC_LOAD 0x0080
#define AFE_CLOCK_EN_RX_PADC_CKIN_160M 0x0100
            uint8_t                b_tunable_delay;
            uint8_t                clk_en;
            uint8_t                coarse_tdc_en;
            uint8_t                outr_en;
            uint8_t                start;
            uint8_t                ftdc_ctrl_f;
            uint8_t                ftdc_ctrl_s;
            struct hw_afe_rfa_rxtdc_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
                uint8_t            bleed_ctrl;
                uint8_t            vref_ctrl;
            } ldo;
            struct hw_afe_rfa_rxtdc_ftdc_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
                uint8_t            en_istdby;
            } ftdc_ldo;
            struct hw_afe_rfa_rxtdc_ring_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
            } ring_ldo;
            uint8_t                sw;
            uint8_t                en_ext_in;
            uint8_t                en_div2;
            uint8_t                en_div4;
            uint8_t                en_div8;
            uint8_t                en_interface;
            uint8_t                course_locked;
            uint8_t                ring_phase[3];
            uint8_t                sel_debug;
            uint8_t                ring_ldo_fc_ctrl;
            uint8_t                ldo_fc_ctrl;
            uint8_t                in_mux_sel;
            uint8_t                ring_ldo_fc_sens;
        } rxtdc;
        struct hw_afe_rfa_tx {
            uint8_t                misc[3];
            struct hw_afe_rfa_tx_dpa {
                uint8_t            pon;
                uint8_t            cas_ctrl;
                uint8_t            mux_pon;
                uint8_t            rstn_640;
                uint8_t            rstn_160;
                struct hw_afe_rfa_tx_dpa_digital_front {
                    uint8_t        enable_sign;
                    uint8_t        enable_interpolate;
                    uint16_t       permute;
                    uint8_t        misc;
                } digital_front;
                uint8_t            ph_tree_pon;
                uint8_t            sw_en;
                uint8_t            pon_pdet;
                uint8_t            pdet;
                uint8_t            pdet_load_mode;
                uint8_t            pdet_load_res_ctrl;
                uint8_t            testbuff_pe;
                uint8_t            lpbk_enb;
            } dpa;
        } tx;
        struct hw_afe_rfa_txph {
            struct hw_afe_rfa_txph_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
                uint8_t            en_istdby;
            } ldo;
        } txph;
        struct hw_afe_rfa_bias {
            uint8_t                pon;
            uint8_t                rtest;
            struct hw_afe_rfa_bias_ldo {
                uint8_t            ctrl;
#define AFE_CTRL_PD                0x01
#define AFE_CTRL_BP                0x02
                uint8_t            vset;
            } ldo;
        } bias;
        uint8_t                    ldoref;
#define AFE_LDOREF_BGR_PD          0x01
#define AFE_LDOREF_TSR_PD          0x02
        struct hw_afe_rfa_hkadc {
            uint8_t                chsel;
            uint8_t                ckdiv;
            uint8_t                vref_sel;
            uint8_t                mode_sd;
            uint16_t               d_out;
            uint8_t                reset;
            uint8_t                frc_on;
            uint8_t                meas_init;
            uint8_t                avg_en;
            uint8_t                ck_inv;
            uint8_t                avg_cnt;
            uint8_t                data_skip;
        } hkadc;
        struct hw_afe_rfa_rct {
            uint8_t                start;
            uint8_t                coff;
            uint8_t                ck_lock;
            uint8_t                code;
        } rct;
        uint8_t                    rx_lna_gain_ble_2;
        struct hw_afe_rfa_dpll_delay {
            uint8_t                cml_gm_en;
            uint8_t                cml_load_en;
            uint8_t                eight_phase_en;
            uint8_t                eight_phase_sel;
        } dpll_delay;
    } rfa;
};

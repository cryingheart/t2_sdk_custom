#pragma once
#include <stdint.h>
#include <kernel/compiler.h>

struct llc {
	uint8_t llc_dsap;
	uint8_t llc_ssap;
        uint8_t llc_control;
        struct {
            uint8_t  org_code[3];
            uint16_t ether_type;
        } __packed llc_snap;
} __packed;

#define LLC_SNAPFRAMELEN 8

/*
 * Unnumbered LLC format commands
 */
#define LLC_UI		0x3
#define LLC_UI_P	0x13
#define LLC_DISC	0x43
#define	LLC_DISC_P	0x53
#define LLC_UA		0x63
#define LLC_UA_P	0x73
#define LLC_TEST	0xe3
#define LLC_TEST_P	0xf3
#define LLC_FRMR	0x87
#define	LLC_FRMR_P	0x97
#define LLC_DM		0x0f
#define	LLC_DM_P	0x1f
#define LLC_XID		0xaf
#define LLC_XID_P	0xbf
#define LLC_SABME	0x6f
#define LLC_SABME_P	0x7f

/*
 * Supervisory LLC commands
 */
#define	LLC_RR		0x01
#define	LLC_RNR		0x05
#define	LLC_REJ		0x09

/*
 * Info format - dummy only
 */
#define	LLC_INFO	0x00

/*
 * ISO PDTR 10178 contains among others
 */
#define LLC_8021D_LSAP	0x42
#define LLC_X25_LSAP	0x7e
#define LLC_SNAP_LSAP	0xaa
#define LLC_ISO_LSAP	0xfe

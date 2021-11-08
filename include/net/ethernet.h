/*
 * Fundamental constants relating to ethernet.
 *
 * $FreeBSD$
 *
 */

#ifndef _NET_ETHERNET_H_
#define _NET_ETHERNET_H_
#include <stdint.h>
#include <kernel/compiler.h>

/*
 * Some basic Ethernet constants.
 */
#define	ETHER_ADDR_LEN		6	/* length of an Ethernet address */
#define	ETHER_TYPE_LEN		2	/* length of the Ethernet type field */
#define	ETHER_CRC_LEN		4	/* length of the Ethernet CRC */
#define	ETHER_HDR_LEN		(ETHER_ADDR_LEN*2+ETHER_TYPE_LEN)
#define	ETHER_MIN_LEN		64	/* minimum frame len, including CRC */
#define	ETHER_MAX_LEN		1518	/* maximum frame len, including CRC */

/*
 * Ethernet CRC32 polynomials (big- and little-endian verions).
 */
#define	ETHER_CRC_POLY_LE	0xedb88320
#define	ETHER_CRC_POLY_BE	0x04c11db6

/*
 * A macro to validate a length with
 */
#define	ETHER_IS_VALID_LEN(foo)	\
	((foo) >= ETHER_MIN_LEN && (foo) <= ETHER_MAX_LEN)

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct ether_header {
	uint8_t         ether_dhost[ETHER_ADDR_LEN];
	uint8_t         ether_shost[ETHER_ADDR_LEN];
	uint16_t        ether_type;
} __packed;

/*
 * Structure of a 48-bit Ethernet address.
 */
struct ether_addr {
	uint8_t         octet[ETHER_ADDR_LEN];
} __packed;

#define	ETHER_IS_MULTICAST(addr) (*(addr) & 0x01) /* is address mcast/bcast? */

/*
 *  NOTE: 0x0000-0x05DC (0..1500) are generally IEEE 802.3 length fields.
 *  However, there are some conflicts.
 */

#define	ETHERTYPE_IP		0x0800	/* IP protocol */
#define	ETHERTYPE_ARP		0x0806	/* Address resolution protocol */
#define	ETHERTYPE_AARP		0x80F3	/* AppleTalk AARP */
#define ETHERTYPE_VLAN		0x8100	/* IEEE 802.1Q VLAN tagging */
#define	ETHERTYPE_IPX		0x8137	/* Novell (old) NetWare IPX (ECONFIG E option) */
#define	ETHERTYPE_IPV6		0x86DD	/* IP protocol version 6 */
#define	ETHERTYPE_PAE		0x888e	/* EAPOL PAE/802.1x */
#define	ETHERTYPE_LOOPBACK	0x9000	/* Loopback: used to test interfaces */

#define	ETHERTYPE_MAX		0xFFFF	/* Maximum valid ethernet type, reserved */

/*
 * The ETHERTYPE_NTRAILER packet types starting at ETHERTYPE_TRAIL have
 * (type-ETHERTYPE_TRAIL)*512 bytes of data followed
 * by an ETHER type (as given above) and then the (variable-length) header.
 */
#define	ETHERTYPE_TRAIL		0x1000		/* Trailer packet */
#define	ETHERTYPE_NTRAILER	16

#define	ETHERMTU	(ETHER_MAX_LEN-ETHER_HDR_LEN-ETHER_CRC_LEN)
#define	ETHERMIN	(ETHER_MIN_LEN-ETHER_HDR_LEN-ETHER_CRC_LEN)

#endif /* !_NET_ETHERNET_H_ */

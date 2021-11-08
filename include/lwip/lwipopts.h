/**---------------------------------------------------------------
 * # Copyright (c) 2017, InnophaseInc
 * # All rights reserved.
 * #
 * # @authors: agonga@InnophaseInc.com,
 * #---------------------------------------------------------------*/

#ifndef LWIP_LWIPOPTS_H
#define LWIP_LWIPOPTS_H


/*
 * -----------------------------------------------
 * ---------- Platform specific locking ----------
 * -----------------------------------------------
 */
#define NO_SYS                          0

#define TCPIP_THREAD_PRIO               4
#define TCPIP_THREAD_STACKSIZE          2*1024
/* Limit maximum segment lifetime to minimize the time tcp_pcb:s are kept in
 * TIME_WAIT.
 */
#define TCP_MSL                         6000UL

/**
 * NO_SYS_NO_TIMERS==1: Drop support for sys_timeout when NO_SYS==1
 * Mainly for compatibility to old versions.
 */
#define NO_SYS_NO_TIMERS                1

/*
 * ------------------------------------
 * ---------- Memory options ----------
 * ------------------------------------
 */

#define MEM_LIBC_MALLOC                 1
#define MEMP_MEM_MALLOC                 1
#define MEM_ALIGNMENT                   1

#define mem_clib_free(_ptr)            os_free((_ptr))
#define mem_clib_malloc(_size)         os_alloc((_size))
#define mem_clib_calloc(_nmemb, _size) os_calloc((_nmemb), (_size))

/**
 * MEM_SIZE: the size of the heap memory. If the application will send
 * a lot of data that needs to be copied, this should be set high.
 */
#define MEM_SIZE                        2600

/*
 *   ------------------------------------------------
 *   ---------- Internal Memory Pool Sizes ----------
 *   ------------------------------------------------
 */
/**
 * MEMP_NUM_PBUF: the number of memp struct pbufs (used for PBUF_ROM and PBUF_REF).
 * If the application sends a lot of data out of ROM (or other static memory),
 * this should be set high.
 */
#define MEMP_NUM_PBUF                   16

/**
 * MEMP_NUM_RAW_PCB: Number of raw connection PCBs
 * (requires the LWIP_RAW option)
 */
#define MEMP_NUM_RAW_PCB                16 //4

/**
 * MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One
 * per active UDP "connection".
 * (requires the LWIP_UDP option)
 */
#define MEMP_NUM_UDP_PCB                16//4

/**
 * MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB                16//8

/**
 * MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_PCB_LISTEN         8

/**
 * MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments.
 * (requires the LWIP_TCP option)
 */
#define MEMP_NUM_TCP_SEG                16

/**
 * MEMP_NUM_ARP_QUEUE: the number of simulateously queued outgoing
 * packets (pbufs) that are waiting for an ARP request (to resolve
 * their destination address) to finish.
 * (requires the ARP_QUEUEING option)
 */
#define MEMP_NUM_ARP_QUEUE              16

/**
 * MEMP_NUM_SYS_TIMEOUT: the number of simultaneously active timeouts.
 * The default number of timeouts is calculated here for all enabled modules.
 * The formula expects settings to be either '0' or '1'.
 *
 * To this default value, 1 was added for the snmp_increment timer.
 */
#define MEMP_NUM_SYS_TIMEOUT            (LWIP_TCP + IP_REASSEMBLY + LWIP_ARP + (2*LWIP_DHCP) + LWIP_AUTOIP + LWIP_IGMP + LWIP_DNS + PPP_SUPPORT + (LWIP_IPV6 ? (1 + LWIP_IPV6_REASS + LWIP_IPV6_MLD) : 0)) + 1

/**
 * MEMP_NUM_NETBUF: the number of struct netbufs.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETBUF                 8//4

/**
 * LWIP_NETBUF_RECVINFO==1: append destination addr and port to every netbuf.
 */
#define LWIP_NETBUF_RECVINFO 1

/**
 * MEMP_NUM_NETCONN: the number of struct netconns.
 * (only needed if you use the sequential API, like api_lib.c)
 */
#define MEMP_NUM_NETCONN                16//8

/**
 * MEMP_NUM_TCPIP_MSG_API: the number of struct tcpip_msg, which are used
 * for callback/timeout API communication.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_API          16//8

/**
 * MEMP_NUM_TCPIP_MSG_INPKT: the number of struct tcpip_msg, which are used
 * for incoming packets.
 * (only needed if you use tcpip.c)
 */
#define MEMP_NUM_TCPIP_MSG_INPKT        8//4

/**
 * PBUF_POOL_SIZE: the number of buffers in the pbuf pool.
 */
#define PBUF_POOL_SIZE                  16




/*
 * ------------------------------------
 * ---------- Socket options ----------
 * ------------------------------------
 */
#define LWIP_COMPAT_SOCKETS             1


/*
 * - -------------------------------------*----------
 * ---------- Network Interfaces options ----------
 * ------------------------------------------------
 */
#define LWIP_NETIF_STATUS_CALLBACK      1
#define LWIP_NETIF_EXT_STATUS_CALLBACK  1
#define LWIP_CHECKSUM_CTRL_PER_NETIF    1

/*
 *   ------------------------------------
 *   ---------- LOOPIF options ----------
 *   ------------------------------------
 */
/**
 * LWIP_NETIF_LOOPBACK==1: Support sending packets with a destination IP
 * address equal to the netif IP address, looping them back up the stack.
 */
#define LWIP_NETIF_LOOPBACK             1
#define LWIP_LOOPBACK_MAX_PBUFS         2
/**
 * NOTE: FOR TEST PURPOSES ONLY
 *
 */
//#define LWIP_HAVE_LOOPIF                1
/**
 * LWIP_LOOPIF_MULTICAST==1: Support multicast/IGMP on loop interface (127.0.0.1).
 */
#define LWIP_LOOPIF_MULTICAST           1

/** END OF TEST PURPOSESONLY */


/*
 *   ---------------------------------
 *   ---------- RAW options ----------
 *   ---------------------------------
 */
/**
 * LWIP_RAW==1: Enable application layer to hook into the IP layer itself.
 */
#define LWIP_RAW                        1
#define DEFAULT_RAW_RECVMBOX_SIZE       4 //0-original

/**
 *   ---------- ARP options ----------
 */
/**
 * LWIP_ARP==1: Enable ARP functionality.
 */
#define LWIP_ARP                        1

/**
 *   --------------------------------
 *   ---------- IPv4 options ----------
 *   --------------------------------
 */

/**
 * IP_FORWARD==1: Enables the ability to forward IP packets across network
 * interfaces. If you are going to run lwIP on a device with only one network
 * interface, define this to 0.
 */
#define IP_FORWARD                      1 //0-original

/**
 * IP_OPTIONS: Defines the behavior for IP options.
 *      IP_OPTIONS==0_ALLOWED: All packets with IP options are dropped.
 *      IP_OPTIONS==1_ALLOWED: IP options are allowed (but not parsed).
 */
#define IP_OPTIONS_ALLOWED              1

/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY                   1 //1-original

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG                         1

/**
 * IP_REASS_MAXAGE: Maximum time (in multiples of IP_TMR_INTERVAL - so seconds, normally)
 * a fragmented IP packet waits for all fragments to arrive. If not all fragments arrived
 * in this time, the whole packet is discarded.
 */
#define IP_REASS_MAXAGE                 3

/**
 * IP_REASS_MAX_PBUFS: Total maximum amount of pbufs waiting to be reassembled.
 * Since the received pbufs are enqueued, be sure to configure
 * PBUF_POOL_SIZE > IP_REASS_MAX_PBUFS so that the stack is still able to receive
 * packets even if the maximum amount of fragments is enqueued for reassembly!
 */
#define IP_REASS_MAX_PBUFS              10

/**
 * IP_FRAG_USES_STATIC_BUF==1: Use a static MTU-sized buffer for IP
 * fragmentation. Otherwise pbufs are allocated and reference the original
 * packet data to be fragmented.
 */
#define IP_FRAG_USES_STATIC_BUF         0

/**
 * IP_DEFAULT_TTL: Default value for Time-To-Live used by transport layers.
 */
#define IP_DEFAULT_TTL                  255


/*
 - --*------------------------------------
 ---------- IPv4/IPv6 ENABLE options ---------------
 ---------------------------------------
 */
/**
 * @defgroup lwip_opts_ipv6 IPv6
 * @ingroup lwip_opts
 * @{
 */
/**
 * LWIP_IPV6==1: Enable IPv6
 */
#define LWIP_IPV6                       1
#if LWIP_IPV6
#define LWIP_BROADCAST_PING             1
#define LWIP_MULTICAST_PING             1
#define LWIP_IPV6_FORWARD               1
#define LWIP_IPV6_DHCP6                 1

#endif /** LWIP_IPV6*/
/**
 * LWIP_IPV4==1: Enable IPv4
 */
#define LWIP_IPV4                       1

/*
 *   ----------------------------------
 *   ---------- ICMP options ----------
 *   ----------------------------------
 */
/**
 * LWIP_ICMP==1: Enable ICMP module inside the IP stack.
 * Be careful, disable that make your product non-compliant to RFC1122
 */
#define LWIP_ICMP                       1

/**
 * ICMP_TTL: Default value for Time-To-Live used by ICMP packets.
 */
#define ICMP_TTL                       (IP_DEFAULT_TTL)


/*
 *   ---------------------------------
 *   ---------- UDP options ----------
 *   ---------------------------------
 */
/**
 * LWIP_UDP==1: Turn on UDP.
 * LWIP_UDPLITE==1: Turn on UDP-Lite. (Requires LWIP_UDP)
 */
#define LWIP_UDP                        1
#define LWIP_UDPLITE                    1
#define DEFAULT_UDP_RECVMBOX_SIZE       64

/*
 *   ----------------------------------
 *   ---------- DHCP options ----------
 *   ----------------------------------
 */
/**
 * LWIP_DHCP==1: Enable DHCP module.
 */
#define LWIP_DHCP                       1
#define DHCP_DOES_ARP_CHECK             0


/*
 * ----------------------------------
 * ---------- IGMP options ----------
 * ----------------------------------
 */
/**
 * LWIP_IGMP==1: Turn on IGMP module.
 */
#define LWIP_IGMP                       1
/*
 * - ---------------------------------
 * ---------- DNS options -----------
 * ----------------------------------
 */
/**
 * LWIP_DNS==1: Turn on DNS module. UDP must be available for DNS
 * transport.
 */
#define LWIP_DNS                        1

/**
 * LWIP_MDNS_RESPONDER == 1 Turn On LWIP_MDNS_RESPONDER module.
 * UDP must be available to transport DNS.
 */
#define LWIP_MDNS_RESPONDER             1
/**
 * LWIP_NUM_NETIF_CLIENT_DATA: Number of clients that may store
 * data in client_data member array of struct netif (max. 256).
 */
#define LWIP_NUM_NETIF_CLIENT_DATA      4

/** The maximum number of services per netif */
#define MDNS_MAX_SERVICES               8 //1-original
/** Set this to 1 to enable querying ".local" names via mDNS
 *  using a One-Shot Multicast DNS Query */
#define LWIP_DNS_SUPPORT_MDNS_QUERIES   1
/*
 *   ------------------------------------
 *   ---------- AUTOIP options ----------
 *   ------------------------------------
 */
/**
 * LWIP_AUTOIP==1: Enable AUTOIP module.
 */
#define LWIP_AUTOIP                     0

/*
 *   ---------------------------------
 *   ---------- TCP options ----------
 *   ---------------------------------
 */
/**
 * LWIP_TCP==1: Turn on TCP.
 */
#define LWIP_TCP                        1
#define TCPIP_MBOX_SIZE                 64
#define DEFAULT_TCP_RECVMBOX_SIZE       64
#define TCP_LISTEN_BACKLOG              1
#define DEFAULT_ACCEPTMBOX_SIZE         8 //0-original
#define LWIP_TCP_SACK_OUT               1
/**
 * TCP_MSS: TCP Maximum segment size. (default is 536, a conservative default,
 * you might want to increase this.)
 * For the receive side, this MSS is advertised to the remote side
 * when opening a connection. For the transmit size, this MSS sets
 * an upper limit on the MSS advertised by the remote host.
 */
#define TCP_MSS                         1460

/**
 * TCP_WND: The size of a TCP window.  This must be at least
 * (2 * TCP_MSS) for things to work well.
 * ATTENTION: when using TCP_RCV_SCALE, TCP_WND is the total size
 * with scaling applied. Maximum window value in the TCP header
 * will be TCP_WND >> TCP_RCV_SCALE
 */
extern int lwip_tcp_wnd;
#define TCP_WND                         (lwip_tcp_wnd)
#define TCP_SND_BUF                     (lwip_tcp_wnd)
#define TCP_SNDLOWAT                    ((2 * TCP_MSS) + 1)
#define LWIP_DISABLE_TCP_SANITY_CHECKS  1
/*
 *  ---------------------------------
 *  ------- OTHER OPTIONS -----------
 *  ---------------------------------
 */

#define LWIP_NETIF_API  1
#define LWIP_COMPAT_MUTEX_ALLOWED 0

/**
 * LWIP_SO_SNDTIMEO==1: Enable send timeout for sockets/netconns and
 * SO_SNDTIMEO processing.
 */
#define LWIP_SO_SNDTIMEO                1 //0-original

/**
 * LWIP_SO_RCVTIMEO==1: Enable receive timeout for sockets/netconns and
 * SO_RCVTIMEO processing.
 */
#define LWIP_SO_RCVTIMEO                1 //0-original
#define LWIP_SO_RCVBUF                  1 //0-original

/**
 * SYS_LIGHTWEIGHT_PROT==1: enable inter-task protection (and task-vs-interrupt
 * protection) for certain critical regions during buffer allocation, deallocation
 * and memory allocation and deallocation.
 * ATTENTION: This is required when using lwIP from more than one context! If
 * you disable this, you must be sure what you are doing!
 */
#define SYS_LIGHTWEIGHT_PROT            1 //1-original

/**
 * SO_REUSE==1: Enable SO_REUSEADDR option.
 */
#define SO_REUSE                        1

/*
 *   ---------------------------------------
 *   ---------- Debugging options ----------
 *   ---------------------------------------
 */
//#define LWIP_DEBUG
#define TAPIF_DEBUG      LWIP_DBG_OFF
#define TUNIF_DEBUG      LWIP_DBG_OFF
#define UNIXIF_DEBUG     LWIP_DBG_OFF
#define DELIF_DEBUG      LWIP_DBG_OFF
#define SIO_FIFO_DEBUG   LWIP_DBG_OFF
#define TCPDUMP_DEBUG    LWIP_DBG_OFF
#define API_LIB_DEBUG    LWIP_DBG_OFF
#define API_MSG_DEBUG    LWIP_DBG_OFF
#define TCPIP_DEBUG      LWIP_DBG_OFF
#define NETIF_DEBUG      LWIP_DBG_OFF
#define SOCKETS_DEBUG    LWIP_DBG_OFF
#define DEMO_DEBUG       LWIP_DBG_OFF
#define IP_DEBUG         LWIP_DBG_OFF
#define IP6_DEBUG        LWIP_DBG_OFF
#define IP_REASS_DEBUG   LWIP_DBG_OFF
#define RAW_DEBUG        LWIP_DBG_OFF
#define ICMP_DEBUG       LWIP_DBG_OFF
#define UDP_DEBUG        LWIP_DBG_OFF
#define TCP_DEBUG        LWIP_DBG_OFF
#define TCP_INPUT_DEBUG  LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG LWIP_DBG_OFF
#define TCP_RTO_DEBUG    LWIP_DBG_OFF
#define TCP_CWND_DEBUG   LWIP_DBG_OFF
#define TCP_WND_DEBUG    LWIP_DBG_OFF
#define TCP_FR_DEBUG     LWIP_DBG_OFF
#define TCP_QLEN_DEBUG   LWIP_DBG_OFF
#define TCP_RST_DEBUG    LWIP_DBG_OFF
#define ETHARP_DEBUG     LWIP_DBG_OFF
#define DHCP_DEBUG       LWIP_DBG_OFF
#define PING_DEBUG       LWIP_DBG_ON
#define MDNS_DEBUG       LWIP_DBG_OFF

#endif //LWIP_LWIPOPTS_H

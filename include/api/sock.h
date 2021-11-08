/*
 * Socket API specificatoin
 */
#pragma once
#include <string.h>
#include <kernel/hostio.h>

/*
[[[cog
import cog, generate
generate.doxygen(generate.Emitter(cog.out), 'sock')
]]] */
/**
 * @addtogroup hioapi_sock
 * @{
 *
 * The sockets interface provides the host with a BSD sockets like interface for
 * implementing network clients and servers.
 *
 * <table>
 * <caption>Message summary (group=5)</caption>
 * <tr><th>OPERATION</th><th>REQ</th><th>RSP</th><th>IND</th></tr>
 * <tr><td>server</td><td>0x00 - sock_server_req</td><td>0x80 - sock_server_rsp</td><td></td></tr>
 * <tr><td>client</td><td>0x01 - sock_client_req</td><td>0x81 - sock_client_rsp</td><td></td></tr>
 * <tr><td>getlocal</td><td>0x02 - sock_getlocal_req</td><td>0x82 - sock_getlocal_rsp</td><td></td></tr>
 * <tr><td>getpeer</td><td>0x03 - sock_getpeer_req</td><td>0x83 - sock_getpeer_rsp</td><td></td></tr>
 * <tr><td>getavailable</td><td>0x04 - sock_getavailable_req</td><td>0x84 - sock_getavailable_rsp</td><td></td></tr>
 * <tr><td>connection</td><td></td><td></td><td>0xc5 - sock_connection_ind</td></tr>
 * <tr><td>close</td><td>0x06 - sock_close_req</td><td>0x86 - sock_close_rsp</td><td>0xc6 - sock_close_ind</td></tr>
 * <tr><td>notification</td><td>0x07 - sock_notification_req</td><td>0x87 - sock_notification_rsp</td><td>0xc7 - sock_notification_ind</td></tr>
 * <tr><td>recvfrom</td><td>0x08 - sock_recvfrom_req</td><td>0x88 - sock_recvfrom_rsp</td><td></td></tr>
 * <tr><td>sendto</td><td>0x09 - sock_sendto_req</td><td>0x89 - sock_sendto_rsp</td><td></td></tr>
 * <tr><td>resolve</td><td>0x0a - sock_resolve_req</td><td>0x8a - sock_resolve_rsp</td><td></td></tr>
 * </table>
 */
/* [[[end]]] */

/*
[[[cog
cog.outl('')
generate.header(generate.Emitter(cog.out), 'sock')
cog.outl('')
]]] */

#define HIO_GROUP_SOCK 5

/*
 * Message server
 */

#define SOCK_SERVER_REQ 0x00
#define SOCK_SERVER_RSP 0x80

/** Create a server socket that binds to the protocol, address and port specified by the URI. Request structure.
 */
struct sock_server_req {
    char uri[64]; /**< URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp */
};

static inline struct packet *
alloc_sock_server_req(struct sock_server_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_server_req(const char uri[64])
{
    struct sock_server_req *req;
    struct packet *pkt = alloc_sock_server_req(&req);
    if (pkt) {
        strncpy(req->uri, uri, 64);
    }
    return pkt;
}

/** Create a server socket that binds to the protocol, address and port specified by the URI. Response structure.
 */
struct sock_server_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to the created socket if successful */
};

static inline struct packet *
alloc_sock_server_rsp(struct sock_server_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_server_rsp(const int32_t status, const uint32_t socket)
{
    struct sock_server_rsp *rsp;
    struct packet *pkt = alloc_sock_server_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->socket = socket;
    }
    return pkt;
}
/*
 * Message client
 */

#define SOCK_CLIENT_REQ 0x01
#define SOCK_CLIENT_RSP 0x81

/** Create a client socket that connects to the protocol, address and port specified by the URI. Request structure.
 */
struct sock_client_req {
    char uri[64]; /**< URI on the form `<protocol>://<host>:<port>` where protocol may be tcp or udp */
};

static inline struct packet *
alloc_sock_client_req(struct sock_client_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_client_req(const char uri[64])
{
    struct sock_client_req *req;
    struct packet *pkt = alloc_sock_client_req(&req);
    if (pkt) {
        strncpy(req->uri, uri, 64);
    }
    return pkt;
}

/** Create a client socket that connects to the protocol, address and port specified by the URI. Response structure.
 */
struct sock_client_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to the created socket if successful */
};

static inline struct packet *
alloc_sock_client_rsp(struct sock_client_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_client_rsp(const int32_t status, const uint32_t socket)
{
    struct sock_client_rsp *rsp;
    struct packet *pkt = alloc_sock_client_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->socket = socket;
    }
    return pkt;
}
/*
 * Message getlocal
 */

#define SOCK_GETLOCAL_REQ 0x02
#define SOCK_GETLOCAL_RSP 0x82

/** Returns the local IPv4/IPv6 address and port. Request structure.
 */
struct sock_getlocal_req {
    uint32_t socket; /**< handle to socket to return name for */
};

static inline struct packet *
alloc_sock_getlocal_req(struct sock_getlocal_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_getlocal_req(const uint32_t socket)
{
    struct sock_getlocal_req *req;
    struct packet *pkt = alloc_sock_getlocal_req(&req);
    if (pkt) {
        req->socket = socket;
    }
    return pkt;
}

/** Returns the local IPv4/IPv6 address and port. Response structure.
 */
struct sock_getlocal_rsp {
    int32_t status; /**< result status, zero is success */
    uint16_t port; /**< local endpoint port */
    uint16_t addrlen; /**< size of the address IPv4(4)/IPv6(16) */
    uint8_t addr[16]; /**< returned local or remote address */
};

static inline struct packet *
alloc_sock_getlocal_rsp(struct sock_getlocal_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_getlocal_rsp(const int32_t status, const uint16_t port, const uint16_t addrlen, const uint8_t addr[16])
{
    struct sock_getlocal_rsp *rsp;
    struct packet *pkt = alloc_sock_getlocal_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->port = port;
        rsp->addrlen = addrlen;
        memcpy(rsp->addr, addr, sizeof rsp->addr);
    }
    return pkt;
}
/*
 * Message getpeer
 */

#define SOCK_GETPEER_REQ 0x03
#define SOCK_GETPEER_RSP 0x83

/** Returns the remote IPv4/IPv6 address and port. Request structure.
 */
struct sock_getpeer_req {
    uint32_t socket; /**< handle to socket to return name for */
};

static inline struct packet *
alloc_sock_getpeer_req(struct sock_getpeer_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_getpeer_req(const uint32_t socket)
{
    struct sock_getpeer_req *req;
    struct packet *pkt = alloc_sock_getpeer_req(&req);
    if (pkt) {
        req->socket = socket;
    }
    return pkt;
}

/** Returns the remote IPv4/IPv6 address and port. Response structure.
 */
struct sock_getpeer_rsp {
    int32_t status; /**< result status, zero is success */
    uint16_t port; /**< remote endpoint port */
    uint16_t addrlen; /**< size of the address IPv4(4)/IPv6(16) */
    uint8_t addr[16]; /**< returned local or remote address */
};

static inline struct packet *
alloc_sock_getpeer_rsp(struct sock_getpeer_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_getpeer_rsp(const int32_t status, const uint16_t port, const uint16_t addrlen, const uint8_t addr[16])
{
    struct sock_getpeer_rsp *rsp;
    struct packet *pkt = alloc_sock_getpeer_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->port = port;
        rsp->addrlen = addrlen;
        memcpy(rsp->addr, addr, sizeof rsp->addr);
    }
    return pkt;
}
/*
 * Message getavailable
 */

#define SOCK_GETAVAILABLE_REQ 0x04
#define SOCK_GETAVAILABLE_RSP 0x84

/** Get the number of bytes available for read. Request structure.
 */
struct sock_getavailable_req {
    uint32_t socket; /**< handle to socket to read data from */
};

static inline struct packet *
alloc_sock_getavailable_req(struct sock_getavailable_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_getavailable_req(const uint32_t socket)
{
    struct sock_getavailable_req *req;
    struct packet *pkt = alloc_sock_getavailable_req(&req);
    if (pkt) {
        req->socket = socket;
    }
    return pkt;
}

/** Get the number of bytes available for read. Response structure.
 */
struct sock_getavailable_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t available; /**< number of bytes available for reading */
};

static inline struct packet *
alloc_sock_getavailable_rsp(struct sock_getavailable_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_getavailable_rsp(const int32_t status, const uint32_t available)
{
    struct sock_getavailable_rsp *rsp;
    struct packet *pkt = alloc_sock_getavailable_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->available = available;
    }
    return pkt;
}
/*
 * Message connection
 */

#define SOCK_CONNECTION_IND 0xc5

/** Notities about a new server connection. Indication structure.
 */
struct sock_connection_ind {
    int32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to socket for the new connection */
};

static inline struct packet *
alloc_sock_connection_ind(struct sock_connection_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_sock_connection_ind(const int32_t status, const uint32_t socket)
{
    struct sock_connection_ind *ind;
    struct packet *pkt = alloc_sock_connection_ind(&ind);
    if (pkt) {
        ind->status = status;
        ind->socket = socket;
    }
    return pkt;
}
/*
 * Message close
 */

#define SOCK_CLOSE_REQ 0x06
#define SOCK_CLOSE_RSP 0x86
#define SOCK_CLOSE_IND 0xc6

/** Closes and destroys the socket specified by the handle. Request structure.
 */
struct sock_close_req {
    uint32_t socket; /**< handle to socket that will be closed and destroyed */
};

static inline struct packet *
alloc_sock_close_req(struct sock_close_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_close_req(const uint32_t socket)
{
    struct sock_close_req *req;
    struct packet *pkt = alloc_sock_close_req(&req);
    if (pkt) {
        req->socket = socket;
    }
    return pkt;
}

/** Closes and destroys the socket specified by the handle. Response structure.
 */
struct sock_close_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_sock_close_rsp(struct sock_close_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_close_rsp(const int32_t status)
{
    struct sock_close_rsp *rsp;
    struct packet *pkt = alloc_sock_close_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Closes and destroys the socket specified by the handle. Indication structure.
 */
struct sock_close_ind {
    int32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle of socket to close */
};

static inline struct packet *
alloc_sock_close_ind(struct sock_close_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_sock_close_ind(const int32_t status, const uint32_t socket)
{
    struct sock_close_ind *ind;
    struct packet *pkt = alloc_sock_close_ind(&ind);
    if (pkt) {
        ind->status = status;
        ind->socket = socket;
    }
    return pkt;
}
/*
 * Message notification
 */

#define SOCK_NOTIFICATION_REQ 0x07
#define SOCK_NOTIFICATION_RSP 0x87
#define SOCK_NOTIFICATION_IND 0xc7

/** Request notification when socket is readable. Request structure.
 */
struct sock_notification_req {
    uint32_t socket; /**< handle to socket that will be closed and destroyed */
    uint32_t threshold; /**< Fire notification when abount of readable data is above threshold */
    uint32_t flags; /**< reserved for future use */
};

static inline struct packet *
alloc_sock_notification_req(struct sock_notification_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_notification_req(const uint32_t socket, const uint32_t threshold, const uint32_t flags)
{
    struct sock_notification_req *req;
    struct packet *pkt = alloc_sock_notification_req(&req);
    if (pkt) {
        req->socket = socket;
        req->threshold = threshold;
        req->flags = flags;
    }
    return pkt;
}

/** Request notification when socket is readable. Response structure.
 */
struct sock_notification_rsp {
    int32_t status; /**< result status, zero is success */
};

static inline struct packet *
alloc_sock_notification_rsp(struct sock_notification_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_notification_rsp(const int32_t status)
{
    struct sock_notification_rsp *rsp;
    struct packet *pkt = alloc_sock_notification_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
    }
    return pkt;
}

/** Request notification when socket is readable. Indication structure.
 */
struct sock_notification_ind {
    int32_t status; /**< result status, zero is success */
    uint32_t socket; /**< handle to socket that can be read */
    uint32_t flags; /**< reserved for future use */
};

static inline struct packet *
alloc_sock_notification_ind(struct sock_notification_ind **ind)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **ind);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *ind = packet_insert_tail(pkt, sizeof **ind);
    }
    return pkt;
}

static inline struct packet *
create_sock_notification_ind(const int32_t status, const uint32_t socket, const uint32_t flags)
{
    struct sock_notification_ind *ind;
    struct packet *pkt = alloc_sock_notification_ind(&ind);
    if (pkt) {
        ind->status = status;
        ind->socket = socket;
        ind->flags = flags;
    }
    return pkt;
}
/*
 * Message recvfrom
 */

#define SOCK_RECVFROM_REQ 0x08
#define SOCK_RECVFROM_RSP 0x88

/** Reads up to `size` data bytes from the socket. May return less than `size`. If not data is available the `actual` size will be 0 and status != 0. Request structure.
 */
struct sock_recvfrom_req {
    uint32_t socket; /**< socket descriptor */
    uint32_t size; /**< number of bytes to receive */
    uint32_t flags; /**< reserved for future use */
};

static inline struct packet *
alloc_sock_recvfrom_req(struct sock_recvfrom_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_recvfrom_req(const uint32_t socket, const uint32_t size, const uint32_t flags)
{
    struct sock_recvfrom_req *req;
    struct packet *pkt = alloc_sock_recvfrom_req(&req);
    if (pkt) {
        req->socket = socket;
        req->size = size;
        req->flags = flags;
    }
    return pkt;
}

/** Reads up to `size` data bytes from the socket. May return less than `size`. If not data is available the `actual` size will be 0 and status != 0. Response structure.
 */
struct sock_recvfrom_rsp {
    int32_t status; /**< result status, zero is success */
    int32_t actual; /**< number of bytes received from the socket */
    uint8_t addr[16]; /**< source address */
    uint16_t addrlen; /**< size of the address IPv4(4)/IPv6(16) */
    uint16_t port; /**< source port */
    uint8_t data[0]; /**< buffer with received data */
};

static inline struct packet *
alloc_sock_recvfrom_rsp(struct sock_recvfrom_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_recvfrom_rsp(const int32_t status, const int32_t actual, const uint8_t addr[16], const uint16_t addrlen, const uint16_t port, const uint8_t data[0])
{
    struct sock_recvfrom_rsp *rsp;
    struct packet *pkt = alloc_sock_recvfrom_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->actual = actual;
        memcpy(rsp->addr, addr, sizeof rsp->addr);
        rsp->addrlen = addrlen;
        rsp->port = port;
        /* data must be set by the caller */
    }
    return pkt;
}
/*
 * Message sendto
 */

#define SOCK_SENDTO_REQ 0x09
#define SOCK_SENDTO_RSP 0x89

/** Writes `size` data bytes into the socket. Used on non-connected sockets. Request structure.
 */
struct sock_sendto_req {
    uint32_t socket; /**< socket descriptor */
    uint32_t size; /**< size of data to write to the socket */
    uint32_t flags; /**< reserved for future use */
    uint8_t addr[16]; /**< destination address */
    uint16_t addrlen; /**< size of the address IPv4(4)/IPv6(16) */
    uint16_t port; /**< destination port */
    uint8_t data[0]; /**< buffer containing the data to write to the socket */
};

static inline struct packet *
alloc_sock_sendto_req(struct sock_sendto_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_sendto_req(const uint32_t socket, const uint32_t size, const uint32_t flags, const uint8_t addr[16], const uint16_t addrlen, const uint16_t port, const uint8_t data[0])
{
    struct sock_sendto_req *req;
    struct packet *pkt = alloc_sock_sendto_req(&req);
    if (pkt) {
        req->socket = socket;
        req->size = size;
        req->flags = flags;
        memcpy(req->addr, addr, sizeof req->addr);
        req->addrlen = addrlen;
        req->port = port;
        /* data must be set by the caller */
    }
    return pkt;
}

/** Writes `size` data bytes into the socket. Used on non-connected sockets. Response structure.
 */
struct sock_sendto_rsp {
    int32_t status; /**< result status, zero is success */
    uint32_t actual; /**< number of bytes written to the socket */
};

static inline struct packet *
alloc_sock_sendto_rsp(struct sock_sendto_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_sendto_rsp(const int32_t status, const uint32_t actual)
{
    struct sock_sendto_rsp *rsp;
    struct packet *pkt = alloc_sock_sendto_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        rsp->actual = actual;
    }
    return pkt;
}
/*
 * Message resolve
 */

#define SOCK_RESOLVE_REQ 0x0a
#define SOCK_RESOLVE_RSP 0x8a

/** Returns the IPv4/IPv6 address of a hostname. Request structure.
 */
struct sock_resolve_req {
    uint8_t family; /**<  Address family of socket IPv4/IPv6 <0,4,6,46,64> default is 0 */
    char hostname[0]; /**< the hostname to resolve */
};

static inline struct packet *
alloc_sock_resolve_req(struct sock_resolve_req **req)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **req);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *req = packet_insert_tail(pkt, sizeof **req);
    }
    return pkt;
}

static inline struct packet *
create_sock_resolve_req(const uint8_t family, const char hostname[0])
{
    struct sock_resolve_req *req;
    struct packet *pkt = alloc_sock_resolve_req(&req);
    if (pkt) {
        req->family = family;
        strncpy(req->hostname, hostname, 0);
    }
    return pkt;
}

/** Returns the IPv4/IPv6 address of a hostname. Response structure.
 */
struct sock_resolve_rsp {
    int32_t status; /**< result status, zero is success */
    uint8_t ipaddr[16]; /**< the IPv4/IPv6 address of the hostname */
    uint16_t addrlen; /**< size of the address IPv4(4)/IPv6(16) */
};

static inline struct packet *
alloc_sock_resolve_rsp(struct sock_resolve_rsp **rsp)
{
    struct packet *pkt = packet_alloc(sizeof(struct hio_msghdr) + sizeof **rsp);
    if (pkt) {
        pfrag_reserve(packet_first_frag(pkt), sizeof(struct hio_msghdr));
        *rsp = packet_insert_tail(pkt, sizeof **rsp);
    }
    return pkt;
}

static inline struct packet *
create_sock_resolve_rsp(const int32_t status, const uint8_t ipaddr[16], const uint16_t addrlen)
{
    struct sock_resolve_rsp *rsp;
    struct packet *pkt = alloc_sock_resolve_rsp(&rsp);
    if (pkt) {
        rsp->status = status;
        memcpy(rsp->ipaddr, ipaddr, sizeof rsp->ipaddr);
        rsp->addrlen = addrlen;
    }
    return pkt;
}

/* [[[end]]] */
/** @} */

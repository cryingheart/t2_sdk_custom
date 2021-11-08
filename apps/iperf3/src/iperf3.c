#define OS_DEBUG PR_LEVEL_INFO
#include <assert.h>
#include <kernel/os.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip6_addr.h>
#include <lwip/ip_addr.h>
#include <lwip/tcp.h>
#include <lwip/netif.h>
#include <lwip/api.h>
#include <json.h>
#include "stringstream.h"

#define IPERF3_DEBUG 0

#if !LWIP_IPV6
#define NETCONN_TCP_IPV6 NETCONN_TCP
#define NETCONN_UDP_IPV6 NETCONN_UDP
#endif

#define debug(_fmt, ...) do { if (IPERF3_DEBUG) os_printf(_fmt, ##__VA_ARGS__); } while (0)

#define COOKIE_SIZE 37
#define IPERF3_LOW_HEAP_TX_LIMIT 15000

struct json_string {
    struct json_parser parser;
    enum json_token lookahead;
    const char *ptr;
};

enum {
    MSG_EVENT = 1,
};

enum state {
    TEST_INITIAL     = 0,
    TEST_START       = 1,
    TEST_RUNNING     = 2,
    RESULT_REQUEST   = 3,
    TEST_END         = 4,
    STREAM_BEGIN     = 5,
    STREAM_RUNNING   = 6,
    STREAM_END       = 7,
    ALL_STREAMS_END  = 8,
    PARAM_EXCHANGE   = 9,
    CREATE_STREAMS   = 10,
    SERVER_TERMINATE = 11,
    CLIENT_TERMINATE = 12,
    EXCHANGE_RESULTS = 13,
    DISPLAY_RESULTS  = 14,
    IPERF_START      = 15,
    IPERF_DONE       = 16,
};

struct iperf_pkt {
    uint32_t sec;
    uint32_t usec;
    uint32_t pseq;
};

/* Pre declare struct iperf3*/
struct iperf3;

struct stream {
    struct list_head list;
    struct netconn *conn;
    ip_addr_t peer_addr;
    ip_addr_t local_ip;
    uint16_t  peer_port;
    uint16_t  local_port;
    uint8_t id;
    uint8_t reverse;
    uint32_t next_pseq;
    struct {
        uint64_t num_bytes;
        uint32_t num_pkts;
        uint32_t num_err;
    } stats;
    void (*stream_transfer)(struct iperf3 *, struct stream *, size_t);
};

struct event_msg {
    struct os_msg    hdr;
    enum netconn_evt event;
    uint16_t         len;
    struct netconn  *conn;
};

static struct iperf3 {
    struct netconn *mother;
    struct netconn *ctrl;
    struct list_head streams;
    uint8_t next_id;
    enum state state;
    size_t pos;
    struct netbuf *inbuf;
    void *sendbuf;
    uint64_t start_time;
    uint32_t num_streams;
    uint32_t bps;
    size_t   writelen;
    uint32_t send_interval;
    bool     reverse;
    bool     bidir;
    bool     bidir_func;
    bool     udp;
    uint8_t  tos;
    uint8_t  extra_info;
} iperf3;

/* store if extra info should be displayed*/
static int iperf3_extra_info = 0;
static int iperf3_assert;
BOOTARG_INT("iperf3.extra_info", "extra info", "print extra information");
BOOTARG_INT("iperf3.assert", "assert or errors", "assert on errors so we get a coredump");

/* something like this should exist in lwip, but doesn't */
void netconn_set_tos(struct netconn *conn, uint8_t tos)
{
    if(conn != NULL && conn->pcb.ip != NULL)
        conn->pcb.ip->tos = tos;
}

static struct os_thread *thread_id;
static void (*iperf3_stream_cb)(bool started);

static struct os_msg *
new_event_msg(struct netconn *conn, enum netconn_evt event, u16_t len)
{
    struct event_msg *msg;

    OS_MSG_ALLOC(msg, MSG_EVENT, 0);
    msg->conn = conn;
    msg->event = event;
    msg->len = len;
    return &msg->hdr;
}

static void
iperf_event_cb(struct netconn *conn, enum netconn_evt event, u16_t len)
{
    if (iperf3.state != TEST_RUNNING)
        return;

    if (event == NETCONN_EVT_RCVPLUS || event == NETCONN_EVT_SENDPLUS)
        os_sendmsg(thread_id, new_event_msg(conn, event, len));
}

static const char *
calc_prefix(uint64_t *val, int radix)
{
    if (*val > radix*radix*radix) {
        *val /= radix;
        *val *= 10;
        *val /= radix;
        *val /= radix;
        return "G";
    } else if (*val > radix*radix) {
        *val /= radix;
        *val *= 10;
        *val /= radix;
        return "M";
    }  else if (*val > radix) {
        *val *= 10;
        *val /= radix;
        return "K";
    }

    return "";
}

static void
iperf3_print_result(struct stream *s, uint64_t duration, bool udp)
{
    uint64_t bytes;
    uint64_t speed;

    duration = duration / SYSTIME_SEC(1ULL); /* converts usec -> sec */
    bytes = s->stats.num_bytes;
    speed = 8*bytes / duration;
    const char *size_prefix = calc_prefix(&bytes, 1024);
    const char *speed_prefix = calc_prefix(&speed, 1000);
    os_printf("[%3d] iperf3[S-%s-%s]: 0.0-%lld sec %lld.%lld %sBytes %lld.%lld %sbits/sec\n",
              s->id, s->reverse?"TX":"RX", udp?"udp":"tcp", duration,
              bytes/10, bytes % 10, size_prefix, speed/10, speed % 10, speed_prefix);
}

size_t
ctrl_avail(const struct iperf3 *i3)
{
    return i3->inbuf != NULL ? netbuf_len(i3->inbuf) - i3->pos : 0;
}

int
ctrl_fill(struct iperf3 *i3)
{
    struct netbuf *buf;
    size_t n;
    err_t err;

    if ((err = netconn_recv(i3->ctrl, &buf)) != ERR_OK)
        return err;
    n = netbuf_len(buf);
    if (!i3->inbuf)
        i3->inbuf = buf;
    else
        netbuf_chain(i3->inbuf, buf);

    return n;
}

size_t
ctrl_read(struct iperf3 *i3, void *buf, size_t len)
{
    size_t n;

    if (ctrl_avail(i3) < len)
        ctrl_fill(i3);
    n = netbuf_copy_partial(i3->inbuf, buf, len, i3->pos);
    i3->pos += n;

    return n;
}

static void
ctrl_set_state(struct iperf3 *i3, enum state new_state)
{
    uint8_t buf = new_state;

    pr_debug("iperf3: Set state %d\n", new_state);
    netconn_write(i3->ctrl, &buf, sizeof buf, NETCONN_COPY);
    i3->state = new_state;
}

static enum state
ctrl_get_state(struct iperf3 *i3)
{
    uint8_t buf;

    ctrl_read(i3, &buf, sizeof buf);
    i3->state = buf;
    return i3->state;
}

static char *
ctrl_read_json(struct iperf3 *i3)
{
    char *buf = NULL;
    size_t len;

    if (ctrl_read(i3, &len, sizeof len) == sizeof len) {
        len = ntohl(len);
        if ((buf = os_alloc(len+1)) != NULL) {
            if (ctrl_read(i3, buf, len) != len) {
                os_free(buf);
                buf = NULL;
            } else {
                buf[len] = '\0';
            }
        }
    }

    return buf;
}

static void
ctrl_write_json(struct iperf3 *i3, char *buf, size_t len)
{
    uint32_t n = htonl(len);
    netconn_write(i3->ctrl, &n, 4, NETCONN_COPY);
    netconn_write(i3->ctrl, buf, len, NETCONN_COPY);
}

static char *
json_result(struct iperf3 *i3, uint64_t duration, const struct os_cpu_usage *cpu, size_t *n)
{
    uint32_t cpu_total = cpu->user + cpu->irq + cpu->idle;
    uint32_t cpu_user = 100 * cpu->user / cpu_total;
    uint32_t cpu_irq = 100 * cpu->irq / cpu_total;
    uint32_t cpu_idle = 100 * cpu->idle / cpu_total;
    struct stringstream str;
    struct stream *s;
    bool first = true;

    str_init(&str);
    str_printf(&str,
               "{"
               "\"cpu_util_total\": %u,"
               "\"cpu_util_user\": %u,"
               "\"cpu_util_system\": %u,"
               "\"sender_has_retransmits\": -1,"
               "\"congestion_used\": \"cubic\",",
               cpu_user + cpu_irq, cpu_user, cpu_irq);

    str_printf(&str, "\"streams\": [");
    os_printf("------------------------------\n");
    os_printf("[ ID]\t\t\tInterval        Transfer     Bitrate \n");
    list_for_each_entry(s, &i3->streams, list) {
        str_printf(&str,
		  "%s{"
		  "\"id\":%d,"
		  "\"bytes\":%llu,"
		  "\"retransmits\":0,"
		  "\"jitter\":0,"
		  "\"errors\":%u,"
		  "\"packets\":%u,"
		  "\"start_time\":0,"
		  "\"end_time\":%llu.%06llu"
		  "}",
                  first ? "" : ",",
		  s->id,
		  s->stats.num_bytes,
                  s->stats.num_err,
		  s->stats.num_pkts,
		  duration/SYSTIME_SEC(1ULL), duration%SYSTIME_MS(1ULL));
        first = false;
        iperf3_print_result(s, duration, i3->udp);
    }
    str_printf(&str, "]}");

    os_printf("User: %llu (%u%%)\n", cpu->user, cpu_user);
    os_printf("IRQ:  %llu (%u%%)\n", cpu->irq,  cpu_irq);
    os_printf("Idle: %llu (%u%%)\n", cpu->idle, cpu_idle);

    return str_string(&str, n);
}

static enum json_token
json_string_next(struct json_string *js)
{
    enum json_token t;

    while (*js->ptr && (t = json_tokenizer(&js->parser, *js->ptr++)) == JSON_NONE)
        ;

    return t;
}

static void
json_string_init(struct json_string *js, const char *input)
{
    json_init(&js->parser);
    js->ptr = input;
    js->lookahead = json_string_next(js);
}

static void
json_string_finish(struct json_string *js)
{
    json_finish(&js->parser);
}

static bool
json_match(struct json_string *js, enum json_token match, char *buf, size_t size)
{
    if (match != js->lookahead)
        return false; /* Parse error */
    if (buf)
	strlcpy(buf, js->parser.token, size);
    js->lookahead = json_string_next(js);
    return true;
}

static int
json_find(const char *input, const char *name, char *value, size_t size)
{
    struct json_string json;
    int found = 0;
    char token[32];

    json_string_init(&json, input);

    if (!json_match(&json, JSON_BEGIN_OBJECT, NULL, 0))
        return 0;

    while (!found && json.lookahead != JSON_END_OBJECT) {
        if (!json_match(&json, JSON_STRING, token, sizeof token))
            break;
        found = strcmp(name, token) == 0;
        if (!json_match(&json, JSON_MEMBER_SEPARATOR, NULL, 0))
            break;
        switch (json.lookahead) {
            case JSON_STRING:
            case JSON_NUMBER:
                json_match(&json, json.lookahead, value, size);
                break;
            case JSON_TRUE:
            case JSON_FALSE:
                value[0] = json.lookahead == JSON_TRUE ? '1' : '0';
                value[1] = 0;
                json_match(&json, json.lookahead, NULL, 0);
                break;
            default:
                pr_debug("Unsupported value for '%s'\n", name);
                break;
        }
        if (json.lookahead != JSON_END_OBJECT)
            if (!json_match(&json, JSON_VALUE_SEPARATOR, NULL, 0))
                break;
    }
    json_string_finish(&json);
    return found;
}

static int
json_find_int(const char *input, const char *name, int deflt)
{
    char value[12];

    if (!json_find(input, name, value, sizeof value))
        return deflt;

    return strtol(value, NULL, 0);
}

static struct stream *
stream_new(struct iperf3 *i3, struct netconn *conn)
{
    struct stream *s = os_zalloc(sizeof(struct stream));
    if (!s)
        return NULL;
    s->id = ++iperf3.next_id;
    if (s->id == 2)
        s->id = ++iperf3.next_id;
    s->conn      = conn;
    s->reverse   = (i3->reverse || i3->bidir_func);
    s->next_pseq = 1;
    netconn_set_tos(conn, i3->tos);
    return s;
}

static void __unused
stream_delete(struct stream *s)
{
    os_free(s);
}

static void
stream_tcp_read(struct iperf3 *i3, struct stream *s, size_t len)
{
    struct netbuf *buf;

    while (netconn_recv(s->conn, &buf) == ERR_OK) {
        s->stats.num_bytes += netbuf_len(buf);
        netbuf_delete(buf);
    }
}

static void
stream_udp_read(struct iperf3 *i3, struct stream *s, size_t len)
{
    struct netbuf *buf;
    struct iperf_pkt pkt;
    char *data;
    uint16_t size;

    while (netconn_recv(s->conn, &buf) == ERR_OK) {
        netbuf_data(buf, (void **)&data, &size);
        memcpy(&pkt, data, sizeof pkt);
        pkt.sec = be32toh(pkt.sec);
        pkt.usec = be32toh(pkt.usec);
        pkt.pseq = be32toh(pkt.pseq);
        if (pkt.pseq > s->next_pseq) {
            s->stats.num_err += pkt.pseq - s->next_pseq;
            pr_debug("iperf3: Lost %u packets\n", pkt.pseq - s->next_pseq);
        } else if (pkt.pseq < s->next_pseq) {
            pr_warn("iperf3: Out-of-order, received #%u, expected #%u\n", pkt.pseq, s->next_pseq);
        }
        s->next_pseq = pkt.pseq + 1;
        s->stats.num_pkts = pkt.pseq;
        s->stats.num_bytes += netbuf_len(buf);
        netbuf_delete(buf);
    }
}

static void
stream_tcp_write(struct iperf3 *i3, struct stream *s, size_t len)
{
    size_t outlen;
    size_t total = 0;
    size_t part;
    err_t err;
    static bool fail_printed = false;

    if (os_avail_heap() < IPERF3_LOW_HEAP_TX_LIMIT) {
        if (!fail_printed) {
            pr_debug("iperf3 tcp: Low memory can't send\n");
            fail_printed = true;
        }
        return;
    }
    fail_printed = false;

    if (!i3->sendbuf) {
        pr_warn("iperf3 tcp: NULL sendbuf\n");
        return;
    }

    do {
        part = min(len, i3->writelen);
        err = netconn_write_partly(s->conn, (const void *)i3->sendbuf, part, NETCONN_COPY, &outlen);
        len -= outlen;
        total += outlen;
        s->stats.num_bytes += outlen;
        s->stats.num_pkts += 1;
    } while (err == ERR_OK && len > 0 && outlen == part);
    debug("send %zu/%zu bytes, conn=%p, err=%d\n", total, i3->writelen, s->conn, err);
}

static void
stream_udp_write(struct iperf3 *i3, struct stream *s, size_t len)
{
    struct iperf_pkt hdr;
    struct netbuf *buf;
    uint32_t now;
    void *data;
    err_t err;
    static bool fail_printed = false;
    uint64_t tx_time = os_systime64();

    now = os_systime();
    if (os_avail_heap() < IPERF3_LOW_HEAP_TX_LIMIT || (buf = netbuf_new()) == NULL) {
        if (!fail_printed) {
            pr_debug("iperf3 udp: Low memory can't send\n");
            fail_printed = true;
        }
        goto done;
    }
    if ((data = netbuf_alloc(buf, i3->writelen)) == NULL) {
        if (!fail_printed) {
            pr_debug("iperf3 udp: No memory can't send\n");
            fail_printed = true;
        }
        netbuf_delete(buf);
        goto done;
    }

    hdr.sec = htonl(tx_time / SYSTIME_SEC(1ULL));
    hdr.usec = htonl(tx_time % SYSTIME_SEC(1ULL));
    hdr.pseq = htonl(s->next_pseq);
    memcpy(data, &hdr, sizeof hdr);
    s->next_pseq++;
    //get_random_bytes(data + sizeof hdr, i3->writelen - sizeof hdr);
    if ((err = netconn_send(s->conn, buf)) != ERR_OK) {
        if (!fail_printed) {
            pr_warn("iperf3: Send failed, err=%d\n", err);
            fail_printed = true;
        }
    } else {
        fail_printed = false;
    }
    netbuf_delete(buf);
    s->stats.num_bytes += i3->writelen;
    s->stats.num_pkts += 1;
    debug("send %u pkts, conn=%p err=%d\n", s->stats.num_pkts, s->conn, err);

done:
    os_request_timeoutmsg(now + i3->send_interval,
                          new_event_msg(s->conn, NETCONN_EVT_SENDPLUS, ~0));
}

static struct stream *
create_tcp_stream(struct iperf3 *i3)
{
    struct netconn *conn;
    struct netbuf *buf;
    struct stream *s;

    if (netconn_accept(i3->mother, &conn) != ERR_OK)
        return NULL;
    /* Receive stream cookie */
    if (netconn_recv(conn, &buf) != ERR_OK){
        netconn_delete(conn);
        return NULL;
    }

    netbuf_delete(buf);
    if ((s = stream_new(i3, conn)) == NULL){
        netconn_delete(conn);
        return NULL;
    }
    netconn_set_nonblocking(conn, 1);

    s->stream_transfer = (i3->reverse || i3->bidir_func)? stream_tcp_write : stream_tcp_read;
    netconn_getaddr(conn, &s->local_ip, &s->local_port, 1);
    netconn_getaddr(conn, &s->peer_addr, &s->peer_port, 0);
    os_printf("[%3d] local %pI4 port %d connected to %pI4 port %d\n",
                    s->id, &s->local_ip, s->local_port, &s->peer_addr, s->peer_port);

    pr_debug("iperf3: TCP stream #%d (%p)\n", s->id, s->conn);

    return s;
}

static struct stream *
create_udp_stream(struct iperf3 *i3)
{
    struct netconn *conn;
    struct netbuf *buf;
    struct stream *s;
    err_t err;

    if ((conn = netconn_new_with_callback(NETCONN_UDP_IPV6, iperf_event_cb)) == NULL)
        return NULL;

    /* Bind same address to another socket. Otherwise,
       BIND will fail. In this way, we can demultiplex UDP <parallel streams>
       (-P option) and <bidirectional streams> (--bidir option)*/
    ip_set_option(conn->pcb.ip, SOF_REUSEADDR);

    if ((err = netconn_bind(conn, IP_ANY_TYPE, 5201)) != ERR_OK) {
        netconn_delete(conn);
        return NULL;
    }
    /* Receive stream cookie */
    if (netconn_recv(conn, &buf) != ERR_OK) {
        netconn_delete(conn);
        return NULL;
    }

    s = stream_new(i3, conn);
    if (!s) {
        netbuf_delete(buf);
        netconn_delete(conn);
        return NULL;
    }

    /* Extract address and port info from current packet*/
    s->peer_addr  = *netbuf_fromaddr(buf);
    s->peer_port  = netbuf_fromport(buf);
#if LWIP_NETBUF_RECVINFO
    s->local_ip   = *netbuf_destaddr(buf);
    s->local_port = netbuf_destport(buf);
#endif /* LWIP_NETBUF_RECVINFO */

    if (netconn_connect(conn, &s->peer_addr, s->peer_port) != ERR_OK) {
        netbuf_delete(buf);
        netconn_delete(conn);
        stream_delete(s);
        return NULL;
    }
    os_printf("[%3d] local %pI4 port %d connected to %pI4 port %d\n",
                    s->id, &s->local_ip, s->local_port, &s->peer_addr, s->peer_port);
    /* Send something back to acknowledge stream setup */
    netconn_send(s->conn, buf);
    netbuf_delete(buf);
    netconn_set_nonblocking(conn, 1);

    s->stream_transfer = (i3->reverse || i3->bidir_func)? stream_udp_write : stream_udp_read;

    pr_debug("iperf3: UDP stream #%d (%p)\n", s->id, s->conn);

    return s;
}

static struct stream *
lookup_stream(const struct iperf3 *i3, struct netconn *conn)
{
    struct stream *s;

    list_for_each_entry(s, &i3->streams, list) {
        if (s->conn == conn)
            return s;
    }

    return NULL;
}

static void
iperf3_dump(struct iperf3 *i3)
{
    struct stream *s;

    list_for_each_entry(s, &i3->streams, list)
    {
        pr_info("iperf3_dump"
                " state=%d"
                " stream=%d"
                " s:num_bytes=%lld"
                " s:num_pkts=%d"
                " s:num_err=%d"
                "\n",
                i3->state,
                s->id,
                s->stats.num_bytes,
                s->stats.num_pkts,
                s->stats.num_err
                );
    }
}

static void
iperf3_test_cleanup(struct iperf3 *i3)
{
    struct stream *s;

    /* Close all traffic streams */
    list_for_each_entry(s, &i3->streams, list) {
        /* UDP/RAW netconns have no close */
        if (!i3->udp)
            netconn_close(s->conn);
        netconn_delete(s->conn);
    }

    /* Drain all event messages */
    {
        struct event_msg *ev;
        while ((ev = (struct event_msg *) os_recvmsg(true)) != NULL) {
            os_msg_release(&ev->hdr);
        }
    }
}

static int
iperf3_test(struct iperf3 *i3)
{
    struct stream *s;
    struct os_cpu_usage u0,u1;
    uint64_t duration;

    /* Read the connection cookie (and discard it) */
    char *cookie = os_alloc(COOKIE_SIZE);
    ctrl_read(i3, cookie, COOKIE_SIZE);
    os_free(cookie);

    /* Get test parameters */
    ctrl_set_state(i3, PARAM_EXCHANGE);
    char *param = ctrl_read_json(i3);
    if (param == NULL)
        return ENOMEM;
    if (iperf3.extra_info)
        pr_info("iperf3: param = %s\n", param);
    i3->num_streams = json_find_int(param, "parallel", 1);
    i3->reverse     = json_find_int(param, "reverse", 0);
    i3->bidir       = json_find_int(param, "bidirectional", 0);
    i3->udp         = json_find_int(param, "udp", 0);
    i3->tos         = json_find_int(param, "TOS", 0);
    i3->writelen    = min(1460, json_find_int(param, "len", 1460));
    i3->bps         = json_find_int(param, "bandwidth", 1<<20);
    i3->send_interval = SYSTIME_SEC(1) * i3->writelen / (i3->bps/8);
    pr_debug("iperf3: send_interval = %u us\n", i3->send_interval);
    os_free(param);

    if ((i3->reverse || i3->bidir) && !i3->udp) {
        if ((i3->sendbuf = os_zalloc(i3->writelen)) == NULL) {
            pr_err("iperf3: Not enough available heap (len=%zu)\n", i3->writelen);
            return ENOMEM;
        }

        get_random_bytes(i3->sendbuf, i3->writelen);
    }

    ctrl_set_state(i3, CREATE_STREAMS);

    for (int i = 0; i < i3->num_streams*(1 + i3->bidir); i++) {
        if (i3->bidir) {
            i3->bidir_func = (i % 2 != 0) ? true : false;
        }
        if (i3->udp){
            s = create_udp_stream(i3);
        }else{
            s = create_tcp_stream(i3);
        }
        if (s == NULL) {
            pr_err("iperf3: Stream creation failed\n");
            return ENOMEM;
        }
        list_add_tail(&i3->streams, &s->list);
        /* Send initial event to start the kick off the stream */
        if (i3->reverse){
            os_sendmsg(thread_id, new_event_msg(s->conn, NETCONN_EVT_SENDPLUS, ~0));
        }else if(i3->bidir){
            if (i3->bidir_func)
                os_sendmsg(thread_id, new_event_msg(s->conn, NETCONN_EVT_SENDPLUS, ~0));
            i3->bidir_func = false;
        }
    }

    ctrl_set_state(i3, TEST_START);
    ctrl_set_state(i3, TEST_RUNNING);

    if (iperf3_stream_cb != NULL)
        iperf3_stream_cb(true);
    i3->start_time = os_systime64();
    os_get_cpu_usage(&u0);

    while (i3->state == TEST_RUNNING) {
        struct event_msg *ev = (struct event_msg *) os_recvmsg(false);
        struct netconn *conn = ev->conn;
        enum netconn_evt event = ev->event;
        size_t len = ev->len;

        os_msg_release(&ev->hdr);

        if (conn == iperf3.mother) {
            /* Don't care */
        } else if (conn == iperf3.ctrl) {
            if (event == NETCONN_EVT_RCVPLUS) {
                ctrl_get_state(i3);
                pr_debug("iperf3: New state %d\n", i3->state);
            }
        }else if ((!i3->reverse && !i3->bidir) && event == NETCONN_EVT_SENDPLUS) {
            /* Don't care */
        } else if (i3->reverse && event == NETCONN_EVT_RCVPLUS) {
            /* Don't care */
        } else if ((s = lookup_stream(i3, conn)) != NULL) {
            if (s->stream_transfer)
                s->stream_transfer(i3, s, len);
        } else {
            pr_warn("iperf3: UNKNOWN_event: conn=%p ev=%d len=%u\n", conn, event, len);
            iperf3_dump(i3);
            if (iperf3_assert) assert(0);
            pr_warn("iperf3: force restart\n");
            iperf3_test_cleanup(i3);
            return EIO;
        }
    }

    duration = os_systime64() - i3->start_time;
    if (iperf3_stream_cb != NULL)
        iperf3_stream_cb(false);

    /* Record CPU usage */
    os_get_cpu_usage(&u1);
    u1.user -= u0.user;
    u1.irq  -= u0.irq;
    u1.idle -= u0.idle;

    iperf3_test_cleanup(i3);

    if (i3->state == TEST_END) {
        /* Read results from client */
        ctrl_set_state(i3, EXCHANGE_RESULTS);
        char *client_result = ctrl_read_json(i3);
        if (iperf3.extra_info)
            pr_info("iperf3: client_result = %s\n", client_result);
        os_free(client_result);

        /* Send server results to client */
        size_t n;
        char *server_result = json_result(i3, duration, &u1, &n);
        ctrl_write_json(i3, server_result, n);
        if (iperf3.extra_info)
            pr_info("iperf3: server_result = %s\n", server_result);
        os_free(server_result);
        ctrl_set_state(i3, DISPLAY_RESULTS);

        /* Wait for client to switch to IPERF_DONE */
        ctrl_get_state(i3);
    }

    pr_debug("iperf3: Finished in state %d\n", i3->state);

    return 0;
}


static void*
iperf3_thread(void *arg)
{
    struct netconn *mother;

    thread_id = os_self();
    iperf3_stream_cb = arg;

    /* Create a new TCP control socket */
    mother = netconn_new_with_callback(NETCONN_TCP_IPV6, iperf_event_cb);
    assert(mother != NULL);
    netconn_bind(mother, IP_ANY_TYPE, 5201);
    netconn_listen(mother);

    /* Fetch if extra info display flag*/
    iperf3_extra_info = (uint8_t)os_get_boot_arg_int("iperf3.extra_info", 0);
    iperf3_assert = (uint8_t)os_get_boot_arg_int("iperf3.assert", 0);

    for (;;) {
        ip_addr_t remote_ip;
        uint16_t  remote_port;
        memset(&iperf3, 0, sizeof iperf3);
        iperf3.mother = mother;
        iperf3.extra_info = iperf3_extra_info;
        list_init(&iperf3.streams);

        os_printf("----------------------------------------\n");
        os_printf("Iperf3 TCP/UDP server listening on %u\n", 5201);
        os_printf("----------------------------------------\n");
        /* Wait for new connection... */
        if (netconn_accept(iperf3.mother, &iperf3.ctrl) != ERR_OK) {
            pr_err("Failed to accept new connection\n");
            break;
        }
        if (netconn_getaddr(iperf3.ctrl,
                            &remote_ip, &remote_port, 0)==ERR_OK) {
            os_printf("Accepted connection from %s port %d\n",
                                    ipaddr_ntoa( &remote_ip), remote_port);
        }
        /* Run test session as server */
        if (iperf3_test(&iperf3) != 0)
            ctrl_set_state(&iperf3, SERVER_TERMINATE);

        /* Clean up streams */
        struct list_head *e, *tmp;
        list_for_each_safe(e, tmp, &iperf3.streams) {
            struct stream *s = list_entry(e, struct stream, list);
            list_del(e);
            os_free(s);
        }

        /* Close control connection */
        netconn_close(iperf3.ctrl);
        netconn_delete(iperf3.ctrl);

        if (iperf3.sendbuf)
            os_free(iperf3.sendbuf);
        if (iperf3.inbuf)
            netbuf_delete(iperf3.inbuf);
    }

    netconn_close(mother);
    netconn_delete(mother);

    return NULL;
}

void
iperf3_server(void(*stream_cb)(bool started))
{
    os_create_thread("iperf3", iperf3_thread, stream_cb, OS_THREADPRI_LO + 2, 4096);
}
/* vim: sw=4 expandtab cino=(0
 */

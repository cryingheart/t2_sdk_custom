/*
 * Serial-to-Wireless Application
 *
 * This application instantiates a number of host interface APIs:
 *  - wcm, for WiFi connection management
 *  - sock, for sockets like application interface
 *  - mdns, for registering and resolving mDNS records.
 *
 * The T2 device will service these interfaces over UART or SPI connection.
 */
#include <kernel/os.h>
#include <api/hio_wcm.h>
#include <api/hio_sock.h>
#include <api/hio_mdns.h>
#include <api/hio_tls.h>
#include "wifi/wcm.h"

extern bool lwip_running(void);
extern void iperf3_server(void(*stream_cb)(bool started));

static struct refcnt throughput_refcnt;

static void restore_pm(struct refcnt *ref)
{
    wcm_hio_set_pm(0, 12, WIFI_PM_DYN_LISTEN_INT);
}

static void stream_cb(bool started)
{
    if (started) {
        if (refcnt_inc(&throughput_refcnt) == 1) {
            wcm_hio_set_pm(1, 48, WIFI_PM_PS_POLL);
        }
    } else {
        refcnt_dec(&throughput_refcnt, restore_pm);
    }
}

int
main(void)
{
    wcm_hio_init();
    sock_hio_init();
    mdns_hio_init();
    /*NOTE: MUST always be called after sock_hio_init()*/
    tls_hio_init();

    restore_pm(NULL);

    os_printf("Serial-to-Wireless: Ready\n");
    /*IPERF requires LWIP to be running, and it happens when wlan_if is created.*/
    while (lwip_running() == false){
        os_msleep(1000);
    }
    iperf3_server(stream_cb);

    while (true) {
        os_msleep(1000);
    }

    return 0;
}

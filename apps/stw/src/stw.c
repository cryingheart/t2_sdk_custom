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
#include <kernel/hostio.h>
#include <api/hio_wcm.h>
#include <api/hio_sock.h>
#include <api/hio_mdns.h>

extern bool lwip_running(void);
extern void iperf3_server(void(*stream_cb)(bool started));

int
stw(void)
{
    wcm_hio_init();
    sock_hio_init();
    mdns_hio_init();
    debug_hio_init();

    os_printf("Serial-to-Wireless: Ready\n");
    /*IPERF requires LWIP to be running, and it happens when wlan_if is created.*/
    while (lwip_running() == false){
        os_msleep(1000);
    }

    iperf3_server(NULL);

    return 0;
}

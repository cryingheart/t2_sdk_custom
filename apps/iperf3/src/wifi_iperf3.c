#define OS_DEBUG PR_LEVEL_INFO
#include <kernel/os.h>
#include <wifi/wcm.h>
#include <wifi/uri.h>
#include <lwip/ip4_addr.h>
#include <lwip/ip6_addr.h>
#include <lwip/ip_addr.h>
#include <lwip/netif.h>
#include "iperf3.h"

struct wcm_handle *iperf3_wcm_handle;

/**
 * powersave: downlink thoughput can in some cases be increased a little at
 * the expense of power consumtion.
 */
static bool powersave;

static void restore_pm(void)
{
    wcm_pm_config(iperf3_wcm_handle, 0, 12, WIFI_PM_DYN_LISTEN_INT);
}

static void stream_cb(bool started)
{
    if (powersave)
        return;
    if (started) {
        wcm_pm_config(iperf3_wcm_handle, 1, 0, WIFI_PM_PS_POLL);
    } else {
        restore_pm();
    }
}

struct os_semaphore connected;

void
iperf3_wcm_notifier(void *ctx, struct os_msg *msg)
{
    switch(msg->msg_type) {
    case WCM_NOTIFY_MSG_CONNECTED:
        os_sem_post(&connected);
        break;
    default:
        break;
    }
    os_msg_release(msg);
}

void
iperf3_disable_udp_checksum(struct wcm_handle *h)
{
    __auto_type netif = wcm_get_netif(h);

    NETIF_SET_CHECKSUM_CTRL(netif,
                            NETIF_CHECKSUM_ENABLE_ALL
                            & ~(NETIF_CHECKSUM_GEN_UDP
                                | NETIF_CHECKSUM_CHECK_UDP));
}

BOOTARG_BOOL("iperf3.disable_udp_checksum",
             "disable validation and generation of UDP checksum");

int
wifi_iperf3(void)
{
    const char *ssid = os_get_boot_arg_str("ssid");
    const char *passphrase = os_get_boot_arg_str("passphrase") ?: NULL;
    const char *sae_password = os_get_boot_arg_str("sae_password") ?: NULL;
    const char *uri_string = os_get_boot_arg_str("wifi_uri") ?: NULL;
    powersave = os_get_boot_arg_int("iperf3.powersave", 0);
    int rval;

    if (uri_string == NULL && ssid == NULL) {
        os_printf("Need to specify ssid and passphrase boot arguments\n");
        return 0;
    }
    if (uri_string != NULL && ssid != NULL) {
        os_printf("Only specify one of \"ssid\" and \"wifi_uri\"\n");
        return 0;
    }

    iperf3_wcm_handle = wcm_create(NULL);
    // Allowing iperf a long msdu lifetime, ensures that packets are not discarded in
    // congested environments.
    // Here we override the default value, 512ms, with 4s if boot arg is not specified
    int msdu_lifetime = os_get_boot_arg_int("wifi.msdu_lifetime", 4000);
    wcm_set_msdu_lifetime(iperf3_wcm_handle, msdu_lifetime);

    restore_pm();

    os_sem_init(&connected, 0);
    wcm_notify_enable(iperf3_wcm_handle, iperf3_wcm_notifier, NULL);

    /* Enable device suspend (deep sleep) via boot argument */
    if (os_get_boot_arg_int("suspend", 0) != 0)
        os_suspend_enable();

    if(uri_string != NULL) {
        struct wifi_uri uri;
        wifi_uri_init(&uri);

        rval = wifi_uri_parse(&uri, uri_string);

        rval = wcm_add_network_uri(iperf3_wcm_handle, &uri);

        wifi_uri_release(&uri);

        if(rval < 0) {
            pr_err("wcm_add_network = %d\n", rval);
            return 0;
        }
    } else {
        rval = wcm_add_network(iperf3_wcm_handle, ssid, NULL, passphrase);
        if(rval < 0) {
            pr_err("wcm_add_network = %d\n", rval);
            return 0;
        }

        if(sae_password != NULL) {
            rval = wcm_set_sae_password(iperf3_wcm_handle, sae_password);
            if(rval < 0) {
                pr_err("wcm_set_sae_password = %d\n", rval);
                return 0;
            }
        }
    }

    rval = wcm_auto_connect(iperf3_wcm_handle, true);
    if(rval < 0) {
        pr_err("wcm_auto_connect = %d\n", rval);
        return 0;
    }

    os_sem_wait(&connected);

    const ip_addr_t *myaddr = netif_ip_addr4(wcm_get_netif(iperf3_wcm_handle));
    os_printf("IPerf3 server @ %pI4\n", myaddr);

    if(os_get_boot_arg_int("iperf3.disable_udp_checksum", 1))
       iperf3_disable_udp_checksum(iperf3_wcm_handle);

    iperf3_server(stream_cb);

    return 0;
}

/* vim: sw=4 expandtab cino=(0
 */

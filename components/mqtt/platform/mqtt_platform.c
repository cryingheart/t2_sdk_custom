#include "mqtt/include/mqtt.h"
#include <errno.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>


int _mqtt_thread_start(struct os_thread **thread, void (*fn)(void*), void* arg)
{
    *thread = os_create_thread("mqtt",
                               (os_entrypoint_t)fn,
                               arg, OS_THREADPRI_LO,
                               1000);

    if(*thread != NULL)
        return 0;
    return -ENOMEM;
}


void _mqtt_timer_countdown_ms(MQTTTimer* timer, unsigned int timeout_ms)
{
    timer->timeout = os_systime() + SYSTIME_MS(timeout_ms);
}


void _mqtt_timer_countdown(MQTTTimer* timer, unsigned int timeout)
{
    timer->timeout = os_systime() + SYSTIME_SEC(timeout);
}


int _mqtt_timer_left_ms(MQTTTimer* timer)
{
    uint32_t now = os_systime();
    int retval;
    if(timer->timeout <= now)
        retval = 0;
    else
        retval = (timer->timeout - now) / SYSTIME_MS(1);
    return retval;
}


char _mqtt_timer_is_expired(MQTTTimer* timer)
{
    uint32_t now = os_systime();
    return time_after_eq(now, timer->timeout);
}


void _mqtt_timer_init(MQTTTimer* timer)
{
    timer->timeout = 0;
}



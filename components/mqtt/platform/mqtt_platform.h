#pragma once

#include <kernel/os.h>
#include <string.h>

typedef struct MQTTTimer
{
    uint32_t timeout;
} MQTTTimer;


void _mqtt_timer_init(MQTTTimer*);

char _mqtt_timer_is_expired(MQTTTimer*);

void _mqtt_timer_countdown_ms(MQTTTimer*, unsigned int);

void _mqtt_timer_countdown(MQTTTimer*, unsigned int);

int _mqtt_timer_left_ms(MQTTTimer*);

int _mqtt_thread_start(struct os_thread**, void (*fn)(void*), void* arg);
 
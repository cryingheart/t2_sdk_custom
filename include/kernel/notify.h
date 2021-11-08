#pragma once

#include <kernel/message.h>
#include <kernel/list.h>

struct os_notify_entry;

typedef void (*os_notification_t)(struct os_notify_entry*, struct os_msg*);

struct os_notify_entry {
    struct list_head   ne_entry;
    os_notification_t  ne_callback;
};

struct os_notify_head {
    struct list_head nh_head;
};


void os_notify_init_head(struct os_notify_head *nh);

void os_notify_init(struct os_notify_entry *entry,
                    os_notification_t callback);

void os_notify_register(struct os_notify_head *nh,
                        struct os_notify_entry *entry);

void os_notify_deregister(struct os_notify_entry *entry);

void os_notify(struct os_notify_head *nh, struct os_msg *msg);

static inline bool os_notify_empty(struct os_notify_head *nh)
{
    return list_empty(&nh->nh_head);
}

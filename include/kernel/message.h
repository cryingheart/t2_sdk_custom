/**
 * @addtogroup message
 * @{
 */
#pragma once

#include <stdbool.h>
#include <kernel/list.h>
#include <kernel/waitq.h>

/**
 * Definition of a message queue
 *
 * @note None of the fields in this struct need to be directly accessed by an
 * application.
 */
struct os_msgq {
    struct list_head queue;
    struct os_waitq  wq;
};

/**
 * IPC message definition.
 *
 * Definition of the message objects that can be passed between the threads
 * using the message interface funcitons
 *
 * @note The only field in this struct that is of any notable use for an
 * application is the `msg_type` field. The other fields should not be
 * accessed.
 */
struct os_msg {
    /** Arbitrary integer representing the message type */
    unsigned int      msg_type;
    struct os_thread *msg_sender;
    struct list_head  msg_list;
};

enum {
    OS_MSG_UNSPECIFIED = 0,
    OS_MSG_CIPHER      = -1
};

/**
 * Allocate a message of the specified type
 *
 * This macro provides a convenient way to allocate a message of a specific type and returning a pointer with that type.
 *
 * @param _msg Pointer variable that will be assigned the allocated message
 * @param _type Message ID as an interger
 * @param _extra Amount of additional buffer space to be allocated (in addition to the size of the message type)
 */
#define OS_MSG_ALLOC(_msg, _type, _extra) \
    (_msg) = (__typeof__(_msg))OS_ERROR_ON_NULL(os_msg_alloc((_type), sizeof(*(_msg)) + (_extra)))

struct os_msg* os_msg_alloc(unsigned int type, size_t size);

void os_msg_release(struct os_msg* msg);

struct os_msg *os_msg_incref(struct os_msg *msg);

struct os_msg* os_msgq_dequeue(struct os_msgq* q);

struct os_msg* os_msgq_pick(struct os_msgq *q, unsigned int msg_type);

bool os_msgq_empty(struct os_msgq* q);

void os_msgq_enqueue(struct os_msgq* q, struct os_msg* msg);

void os_msgq_init(struct os_msgq* q);

/** @} */

/**
 * @addtogroup hio
 * @{
 */
#pragma once
#include <kernel/os.h>
#include <kernel/uart.h>
#include <kernel/gpio.h>
#include <kernel/workqueue.h>
#include <kernel/packet.h>

enum hio_irq_mode {
    HIO_IRQ_MODE_NONE,
    HIO_IRQ_MODE_HIGH,
    HIO_IRQ_MODE_TOGGLE
};

typedef void (*hio_in_pkt_hook)(struct packet *pkt);
typedef void (*hio_out_pkt_hook)(struct packet *pkt);

struct hio_device {
    void               *hio_priv;
    struct pqueue       hio_outq;
    struct pqueue       hio_inq;
    struct os_work      hio_work_input;
    struct os_work      hio_work_output;
    size_t              hio_maxsize;
    os_gpio_set_t       hio_wakeup_pin;
    uint8_t             hio_wakeup_level;
    enum hio_irq_mode   hio_irq_mode;
    os_gpio_set_t       hio_irq_pin;

    void (*hio_output)(struct hio_device *dev);
    void (*hio_suspend)(struct hio_device *dev);
    void (*hio_resume)(struct hio_device *dev);
    void (*hio_irqack)(struct hio_device *dev);
    void (*hio_sleep)(struct hio_device *dev);
    void (*hio_wakeup)(struct hio_device *dev);
};

/**
 * Definition of HIO message header. This is the common header that preceeds
 * all messages to and from the host interface transport. The header containts
 * fields that multiplex several interface groups over the same channel. The
 * `group` field is the identifier used for this multiplexing. The `msgid` is
 * the selector for the specific function within a group. Messages originating
 * from the host to the device are called 'requests'. A message sent by the
 * device to the host as direct respone to a request is called 'response'. An
 * asynchronous message sent by the device to the host is called an
 * 'indication'.
 *
 * @note The direction is encoded in the two most significant bits of the
 * `msgid`. This encoding is nothing the application needs to be aware of.  All
 * messages are referenced using symbolic names from the header files that
 * define the interface.
 */
struct hio_msghdr {
    /** Group ID */
    uint8_t  group;
    /** Message ID */
    uint8_t  msgid;
    /** Transaction ID. The transaction ID is a sequence number that may be use
     * by the host to match a reply to a specific request. The device will
     * ensure that each reply message will have the same transaction ID as the
     * corresponding request.
     */
    uint16_t trxid;
};

/**
 * Definition of a HIO intefcace API.
 */
struct hio_api {
    /** Group ID that this interface implements */
    uint16_t group;
    /** Number of interface messages in this interface (number of entries in
     * the array below) */
    uint16_t num_handlers;
    /** Function pointers to the handler for each message defined by this
     * interface */
    struct packet *(* const handler[])(void *ctx, struct packet *pkt);
};

void hio_packet_input(struct hio_device *dev, struct packet *pkt);
void hio_write_msg(struct packet *msg, uint32_t group, uint32_t msgid, uint32_t trxid);
void hio_queue_output(struct packet *msg);
int hio_printf(const char *fmt, ...);
struct packet *hio_response_status(int32_t status);
void hio_api_init(const struct hio_api *api, void *ctx);

int hio_api_register(const struct hio_api *api, void *ctx)
    __attribute__((warn_unused_result));

void hio_api_set_maxsize(size_t maxsize);
void hio_send_wakeup_ind(void);
void hio_not_used(struct hio_device *dev);
void hio_irq_assert(struct hio_device *dev);
void hio_irq_deassert(struct hio_device *dev);

void debug_hio_init(void);
void hio_uart_init(struct hio_device *dev);
void hio_sspi_init(struct hio_device *dev);

int hio_packet_hook_register(hio_in_pkt_hook  input_hook_fn,
                             hio_out_pkt_hook output_hook_fn);
void hio_packet_hook_unregister(void);
/** @} */

#include <kernel/hwreg.h>
#include <kernel/boot.h>
#include <kernel/cpu.h>
#include <kernel/interrupt.h>
#include <kernel/secureboot.h>
#include <kernel/endian.h>
#include <sha2.h>

#define DEFAULT_FLASH_SPI_FREQ 40000000

#define CALLROM(fn) ((__typeof__(&(fn)))ROM_##fn)
#define ROM_boot_console_puts (0x23dcd+4)
#define ROM_boot_console_puthex (0x23e2d+4)
#define ROM_dbg_console_putc (0x25d91+4)
#define ROM_secureboot_puf_reconstruct (0x249ad+4)
#define ROM_os_irq_disable (0x21381+4)
#define ROM_os_irq_clear_pending (0x213a9+4)
#define ROM_boot_console_opcode (0x23e25+4)
#define ROM_boot_run_desc (0x2437d+4)
#define ROM_boot_read_flash (0x243a5+4)
#define ROM_boot_flash (0x243f9+4)
#define ROM_secureboot_decrypt (0x2498d+4)
#define ROM_secureboot_puf_reconstruct (0x249ad+4)
#define ROM_secureboot_hash_add (0x24c5d+4)
#define ROM_secureboot_hash_verify (0x24c89+4)
#define ROM_secureboot_init (0x24cbd+4)
#define ROM_memcpy (0x38495+4)
#define ROM_os_alloc (0x1a9bd+4)
#define ROM_os_kernel_init (0x1a599+4)
#define ROM_os_irq_enable (0x21395+4)

#define ROM_mount (0x2300d+4)
#define ROM_file_open (0x2315d)
#define ROM_file_read (0x233c1)
#define ROM_file_write (0x23d31)
#define ROM_file_close (0x22c99)

#define ROM_os_flash_init (0x1fd05+4)
#define ROM_os_flash_get_spi_dev (0x1fd71+4)
#define ROM_boot_spi_set_freq (0x23d51+4)

#define SPIFLASH_OP_READ        0x03000000      /* Read data bytes (low frequency) */
#define SPIFLASH_OP_READ_FAST   0x0b000000      /* Read data bytes (high frequency) */
#define SPIFLASH_OP_READ_FAST_2 0x3b000000      /* Read data bytes (Dual SPI) */
#define SPIFLASH_OP_READ_FAST_4 0x6b000000      /* Read data bytes (Quad SPI) */


struct boot_howto {
#define BOOT_FLASH  BIT(0)
#define BOOT_SECURE BIT(1)
#define BOOT_UART   BIT(2)
#define BOOT_SPI    BIT(3)
#define BOOT_SDIO   BIT(4)
    uint32_t method;
} boot_howto;

#define DMA_FOREACH(_desc, _head)\
    for((_desc) = (_head); (_desc) != NULL; (_desc) = (_desc)->next)

void os_kernel_init(int argc, char **argv);
void dbg_console_putc(int c);
void boot_console_puts(const char *s);
void boot_console_puthex(unsigned int value, int minwidth);
void boot_console_opcode(uint32_t op);
void boot_run_desc(volatile struct hw_dma_ctrl *dmac, int irqno, struct dma_desc *desc);
void boot_read_flash(uint32_t offset, void *dst, size_t len);
void boot_flash(struct boot_args *args, uint32_t offset, struct secureboot_context *secure_ctx);
void os_irq_enable(uint32_t irqno, uint32_t pri);

static void boot_set_spi_freq(unsigned int spi_freq);
uint32_t os_timer_read(os_timer_id_t timer);

void secureboot_decrypt(struct secureboot_context *ctx, uint32_t addr, size_t len);
int  secureboot_puf_reconstruct(uint32_t *secret, uint32_t a0, uint32_t a1);
void secureboot_hash_add(struct secureboot_context *ctx, uint32_t addr, size_t len);
int  secureboot_hash_verify(struct secureboot_context *ctx);

struct boot_args args;
struct boot_cmd cmd;

uint32_t secureboot_secret[9];

void fast_boot_read_flash(uint32_t offset, void *dst, size_t len)
{
    struct dma_desc data;

    dma_map_buffer(&data, dst, len, DESC_RX | DESC_IRQ);
    dma_link(&data, NULL);
    while (mmio_rd8(&hw_qspi->busy))
        ;
    mmio_wr16(len-1, &hw_qspi->data_size);
    mmio_wr32(htobe32(SPIFLASH_OP_READ_FAST_4 | offset), (void*)&hw_qspi->cmd);

    mmio_wr32((QSPI_WIDTH_WIDTH_1 | QSPI_DIRECTION_OUT) * 0x01010101, (volatile uint32_t *)&hw_qspi->setup.cfg[0]);
    mmio_wr8((QSPI_WIDTH_WIDTH_1 | QSPI_DIRECTION_NONE | QSPI_MODE_LAST), &hw_qspi->setup.cfg[4].mode);

    mmio_wr8((QSPI_WIDTH_WIDTH_4 | QSPI_DIRECTION_IN), &hw_qspi->setup.cfg[8].mode);

    mmio_wr8(1, &hw_qspi->start);

    CALLROM(boot_run_desc)(&hw_qspi->dma_ctrl, IRQ_SPI_DMA, &data);
}

void fast_boot_flash(struct boot_args *args, uint32_t offset, struct secureboot_context *secure_ctx)
{
    CALLROM(os_irq_enable)(IRQ_SPI_DMA, 0);
    do {
        //boot_read_flash(offset, &cmd, sizeof cmd);
        //CALLROM(boot_read_flash)(offset, &cmd, sizeof cmd);
        fast_boot_read_flash(offset, &cmd, sizeof cmd);
        if (cmd.sync != BOOT_SYNC) {
	    if (secure_ctx)
		asm("wfe");
	    else
		break;
	}

        boot_howto.method |= BOOT_FLASH;
        if(secure_ctx != NULL)
            boot_howto.method |= BOOT_SECURE;

        offset += sizeof cmd;

        CALLROM(boot_console_opcode)(cmd.op);

        switch (cmd.op) {
            case BOOT_CMD_NOP:
            case BOOT_CMD_READ:
                break;
            case BOOT_CMD_WRITE:
                if (is_ram_address(cmd.addr) || (secure_ctx == NULL)) {
                    //CALLROM(boot_read_flash)(offset, (void *)cmd.addr, cmd.len);
                    fast_boot_read_flash(offset, (void *)cmd.addr, cmd.len);
		    if (secure_ctx) {
			CALLROM(secureboot_decrypt)(secure_ctx, cmd.addr, cmd.len);
			CALLROM(secureboot_hash_add)(secure_ctx, cmd.addr, cmd.len);
		    }
                }
                offset += cmd.len;
                break;

            case BOOT_CMD_ENTRY:
                args->entry = (void*)cmd.addr;

		if (secure_ctx) {
		    CALLROM(secureboot_hash_add)(secure_ctx, cmd.addr, 0);

		    if (CALLROM(secureboot_hash_verify)(secure_ctx)) {
			CALLROM(boot_console_puts)("\nSignature Failed\n");
			asm("wfe");
		    } else {
			CALLROM(boot_console_puts)("\nSignature Match\n");
		    }
		}
                break;

            case BOOT_CMD_SPEED:
                //CALLROM(boot_set_spi_freq)(cmd.addr);
                break;

	    case BOOT_CMD_ARGS:
		if (secure_ctx) {
		    CALLROM(secureboot_hash_add)(secure_ctx, cmd.addr, 0);
		    CALLROM(secureboot_hash_add)(secure_ctx, cmd.len, 0);
		}
                args->argc = cmd.len;
                args->argv = (void*)cmd.addr;
                break;

            case BOOT_CMD_SIGN:
                //CALLROM(boot_read_flash)(offset, secure_ctx->signature, sizeof secure_ctx->signature);
                fast_boot_read_flash(offset, secure_ctx->signature, sizeof secure_ctx->signature);
                offset += cmd.len;
                break;

            case BOOT_CMD_FILL:
                break;

            case BOOT_CMD_PUF:
                CALLROM(secureboot_puf_reconstruct)(secureboot_secret, 0x70000, 0x71000);
                break;

        }
    } while (cmd.op != BOOT_CMD_ENTRY);

    CALLROM(os_irq_disable)(IRQ_SPI_DMA);
    CALLROM(os_irq_clear_pending)(IRQ_SPI_DMA);
    CALLROM(os_irq_clear_pending)(IRQ_SPI);
    CALLROM(boot_console_puts)("\n");   
}

void __naked
main()
{
    os_irqstate_t flags = cpu_irq_disable();

    //volatile struct hw_pmu   *hw_pmu   = (struct hw_pmu*)   0xfc0100;
    //volatile struct hw_timer *hw_timer = (struct hw_timer*) 0xfc0400;
    volatile struct hw_qspi  *hw_qspi  = (struct hw_qspi*)  0xfc0800;

    //CALLROM(boot_console_puts)("fastboot\n");
    mmio_wr8(20, &hw_qspi->setup.speed);
    fast_boot_flash(&args, 0x2000, (void*)0);

    /* Need to cleanup some irq stuff */
    CALLROM(os_irq_disable)(IRQ_CIPHER_RX);
    CALLROM(os_irq_clear_pending)(IRQ_CIPHER_RX);

    cpu_irq_restore(flags);

    args.entry(args.argc, args.argv);
}


#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <kernel/os.h>
#include <kernel/hostio.h>
#include <kernel/spi-mem.h>
#include <kernel/interrupt.h>
#include <kernel/hwreg.h>
#include <kernel/secureboot.h>
#include "gordon.h"
#include "flash-api.h"

void gordon_hio_init(void);
void storage_hio_init(void);

int
main(void)
{
    os_printf("flash: Gordon ready!\n");
    sector_cache_init();
    gordon_hio_init();
    debug_hio_init();
    storage_hio_init();
    os_park_thread(os_self());
    return 0;
}

/**
  ******************************************************************************
  * @file   fota_test.c
  *
  * @brief  sample application to trigger fota process
  *
  ******************************************************************************
  * @attention
  *
  *
  *  Copyright (c) 2021, InnoPhase, Inc.
  *
  *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  *  POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <kernel/os.h>
#include "fota.h"
#include "fota_utils.h"
#include "fs_utils.h"
#include <kernel/boot.h>
#include <kernel/callout.h>
#include <kernel/io.h>
#include <kernel/hwreg.h>
#include <kernel/watchdog.h>

#define FS_ADDR	0x1c0000
#define FS_SIZE	0x40000
#define OTA_STANDALONE  0
#define OTA_UPGRADE     2

struct ota_config *ota_config;
struct ota_config fota_config;

static void
reset_device()
{
    uint8_t trip = 0;
    mmio_wr8(1, &hw_wdg->unlock);
    mmio_wr8(1, &hw_wdg->run);
    mmio_wr8_set(PMU_RST_MASK_POR , &hw_pmu->rst_mask[1]);
    mmio_wr8_clr(PMU_RST_MASK_POR , &hw_pmu->rst_mask[2]);
    trip |= WDG_TRIP_RESET;
    mmio_wr8(trip, &hw_wdg->trip);
    mmio_wr16(0, &hw_wdg->preload);
}

/** Triggers FOTA process **/
void trigger_fota_update()
{
    //modify ota file with AP credentials and server details
    ota_config = &fota_config;

    char *ssid = "inno_test";
    char *pass = "inno_test";
    char *server = "fotaimages.s3-us-west-2.amazonaws.com";
    char *server_port = "443";
    char *name = "wifi_connect";
    char *version ="2-2";
    char *filename = "/wifi_connect_05102021.elf.strip";
    char *hash	= "f2680d3dac64e8fb9eb8b57aa075ec4b83c6d5db940f0fb1c0da876578f21cc4";
    memcpy(ota_config->ssid, ssid, strlen(ssid));
    memcpy(ota_config->passphrase, pass, strlen(pass));
    memcpy(ota_config->server, server, strlen(server));
    memcpy(ota_config->server_port, server_port, strlen(server_port));
    memcpy(ota_config->version, version, strlen(version));
    memcpy(ota_config->name, name, strlen(name));
    memcpy(ota_config->version, version, strlen(version));
    memcpy(ota_config->filename, filename, strlen(filename));
    memcpy(ota_config->hash, hash, strlen(hash));
 
    ota_config->ota_flag=OTA_UPGRADE;
    modify_ota_file(ota_config);

    modify_boot_file(0);
    reset_device();
}

int main(void)
{
    int rc = 0;
    
    os_printf("Hello FOTA\n");

    os_printf("Mounting file system\n");
    rc = mount_ufs();

    if(rc !=0)
    {
        os_printf("Failed to mount filesystem\n");
        goto fota_error;
    }

    os_printf("Triggering Fota..\n");
    for(int i = 5; i >0; i--)
    {
        os_printf("%d\n", i);
        os_msleep(1000);
    }

    trigger_fota_update();
    return 0;

    fota_error:
        os_printf("ERRROR!\n");

    return 0;
}

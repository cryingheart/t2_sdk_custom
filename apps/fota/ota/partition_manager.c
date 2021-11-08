/**
  ******************************************************************************
  * @file   partition_manageer.c
  *
  * @brief  helper functions for partition config file management
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
#include "part.h"

int next_free_sector(struct part_config* part_config)
{
    int top_sector = 0;
    int free_sector = 0;
    for(int i =0; i<part_config->image_count; i++)
    {
        if(part_config->image_config[i].sector > top_sector)
        {
            top_sector = part_config->image_config[i].sector;
            free_sector = top_sector + part_config->image_config[i].sector_count;
        }
    }
    return free_sector;
}

int avail_sectors(struct part_config* part_config)
{
    return((0x1F0000/0x1000)-next_free_sector(part_config));
}


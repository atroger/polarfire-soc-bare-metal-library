/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * code running on e51
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "mpfs_hal/mss_hal.h"
#include "mpfs_hal/mss_mpu.h"
#include "drivers/mss_mmc/mss_mmc.h"
#include "drivers/mss_mmc/mss_mmc_internal_api.h"

#define ERROR_INTERRUPT         0x8000u
#define TRANSFER_COMPLETE       0x1u
#define SECT_NO                 190u
#define BUFFER_A_SIZE           4096u

uint8_t g_mmc_rx_buff[BUFFER_A_SIZE];
uint8_t g_mmc_tx_buff[BUFFER_A_SIZE];

static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint64_t size);
void transfer_complete_handler(uint32_t status);

/******************************************************************************
 *
 */
void e51(void)
{

    mss_mmc_cfg_t g_mmc;
    mss_mmc_status_t ret_status = MSS_MMC_NO_ERROR;
    uint32_t loop_count;
    uint8_t error = 0u;
    uint32_t sector_number = SECT_NO;

    SYSREG->SUBBLK_CLOCK_CR = 0xffffffff;        /* all clocks on */
    SYSREG->SOFT_RESET_CR &= ~( (1u << 0u) |  (1u << 3u) | (1u << 4u) |
                    (1u << 5u) | (1u << 19u) | (1u << 23u) | (1u << 28u));


      /*--------------------------------------------------------------------------
       * Initialize the write and read Buffers
      */
    for(loop_count = 0u; loop_count < (BUFFER_A_SIZE); loop_count++)
    {
        g_mmc_tx_buff[loop_count] = 0x45u + loop_count;
        g_mmc_rx_buff[loop_count] = 0x00u;
    }

    PLIC_init();
    __enable_irq();
    PLIC_SetPriority(MMC_main_PLIC, 2u);
    PLIC_SetPriority(MMC_wakeup_PLIC, 2u);

    loop_count = MSS_MPU_configure(MSS_MPU_MMC,
                                   MSS_MPU_PMP_REGION3,
                                   0x08000000u,
                                   0x200000u,
                                   MPU_MODE_READ_ACCESS|MPU_MODE_WRITE_ACCESS|MPU_MODE_EXEC_ACCESS,
                                   MSS_MPU_AM_NAPOT,
                                   0u);
    /* Configure eMMC */
    g_mmc.clk_rate = MSS_MMC_CLOCK_50MHZ;
    g_mmc.card_type = MSS_MMC_CARD_TYPE_MMC;
    g_mmc.bus_speed_mode = MSS_MMC_MODE_SDR;
    g_mmc.data_bus_width = MSS_MMC_DATA_WIDTH_4BIT;

    ret_status = MSS_MMC_init(&g_mmc);
    if(ret_status == MSS_MMC_INIT_SUCCESS)
    {
          ret_status = MSS_MMC_sdma_write(g_mmc_tx_buff, sector_number, BUFFER_A_SIZE);
          if(ret_status == MSS_MMC_TRANSFER_IN_PROGRESS)
          {
              ret_status = MSS_MMC_pause_sdma_write_hpi(g_mmc_tx_buff, sector_number, BUFFER_A_SIZE);
              if(ret_status == MSS_MMC_TRANSFER_SUCCESS)
              {
                  ret_status = MSS_MMC_resume_sdma_write_hpi();
              }
          }

          if(ret_status == MSS_MMC_TRANSFER_IN_PROGRESS)
          {
              do
              {
                  ret_status = MSS_MMC_get_transfer_status();
              }while(ret_status == MSS_MMC_TRANSFER_IN_PROGRESS);
          }
    }

    if(ret_status == MSS_MMC_TRANSFER_SUCCESS)
    {
        /* Single or Multi Block - Read */
        ret_status = MSS_MMC_sdma_read(sector_number, g_mmc_rx_buff, BUFFER_A_SIZE);
        if(ret_status == MSS_MMC_TRANSFER_IN_PROGRESS)
        {
            do
            {
                ret_status = MSS_MMC_get_transfer_status();
            }while(ret_status == MSS_MMC_TRANSFER_IN_PROGRESS);
        }

        if(ret_status == MSS_MMC_TRANSFER_SUCCESS)
        {
            error = verify_write(g_mmc_tx_buff, g_mmc_rx_buff, BUFFER_A_SIZE);
        }

    }
    while(1u);
}

static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint64_t size)
{
    uint8_t error = 0u;
    uint32_t index = 0u;

    while(size != 0u)
    {
        if(write_buff[index] != read_buff[index])
        {
            error = 1u;
            break;
        }
        index++;
        size--;
    }

    return error;
}

void transfer_complete_handler(uint32_t status)
{
    uint32_t isr_err;

    if(ERROR_INTERRUPT & status)
    {
        isr_err = status >> 16u;

    }
    else if(TRANSFER_COMPLETE & status)
    {
        isr_err = 0u;
    }
    else
    {

    }

}

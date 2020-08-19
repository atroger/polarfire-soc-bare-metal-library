/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on E51
 *
 */

#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"

#include "drivers/mss_mmc/mss_mmc.h"

#define ERROR_INTERRUPT         0x8000u
#define TRANSFER_COMPLETE       0x1u
#define CQ_INTERRUPT            0x4000u
#define SECT_NO                 180u
#define BUFFER_A_SIZE           4096u
#define TASK_ID_WRITE           1u
#define TASK_ID_READ            2u

uint8_t g_mmc_rx_buff[BUFFER_A_SIZE];
uint8_t g_mmc_tx_buff[BUFFER_A_SIZE];


static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint32_t size);
void transfer_complete_handler(uint32_t status);

/* Main function for the hart0(E51 processor).
 * Application code running on hart0 is placed here.
 */
void e51(void)
{
    mss_mmc_cfg_t g_mmc;
    mss_mmc_status_t ret_status;
    uint32_t loop_count;
    uint8_t error = 0u;
    uint32_t sector_number = SECT_NO;

    SYSREG->SUBBLK_CLOCK_CR = 0xffffffff;        // all clocks on
    SYSREG->SOFT_RESET_CR &= ~( (1u << 0u) | (1u << 3u) | (1u << 4u) | (1u << 5u)
                                    | (1u << 19u) | (1u << 23u) | (1u << 28u));

    /*--------------------------------------------------------------------------
     * Initialize the write and read Buffers
     */
    for (loop_count = 0u; loop_count < (BUFFER_A_SIZE); loop_count++)
    {
        g_mmc_tx_buff[loop_count] = 0x45u + loop_count;
        g_mmc_rx_buff[loop_count] = 0x00u;
    }

    PLIC_init();
    __enable_irq();
    PLIC_SetPriority(MMC_main_PLIC, 2u);
    PLIC_SetPriority(MMC_wakeup_PLIC, 2u);

    /* DMA init for eMMC */
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
    g_mmc.bus_voltage = MSS_MMC_3_3V_BUS_VOLTAGE;

    ret_status = MSS_MMC_init(&g_mmc);
    if (ret_status == MSS_MMC_INIT_SUCCESS)
    {
        ret_status = MSS_MMC_cq_init();
        if (ret_status == MSS_MMC_INIT_SUCCESS)
        {
            MSS_MMC_set_handler(transfer_complete_handler);

            ret_status = MSS_MMC_cq_write(g_mmc_tx_buff, sector_number, BUFFER_A_SIZE);
            if (ret_status == MSS_MMC_TRANSFER_IN_PROGRESS)
            {
                do
                {
                    ret_status = MSS_MMC_get_transfer_status();
                }while (ret_status == MSS_MMC_TRANSFER_IN_PROGRESS);
            }
        }
        if (ret_status == MSS_MMC_TRANSFER_SUCCESS)
        {
            ret_status = MSS_MMC_cq_read(sector_number, g_mmc_rx_buff, BUFFER_A_SIZE);
            if (ret_status == MSS_MMC_TRANSFER_IN_PROGRESS)
            {
                do
                {
                    ret_status = MSS_MMC_get_transfer_status();
                }while (ret_status == MSS_MMC_TRANSFER_IN_PROGRESS);
            }

            if (ret_status == MSS_MMC_TRANSFER_SUCCESS)
            {
                error = verify_write(g_mmc_tx_buff, g_mmc_rx_buff, BUFFER_A_SIZE);
            }
        }
    }
    while (1)
    {

    }
}
/******************************************************************************/
static uint8_t verify_write(uint8_t* write_buff, uint8_t* read_buff, uint32_t size)
{
    uint8_t error = 0u;
    uint32_t index = 0u;

    while (size != 0u)
    {
        if (write_buff[index] != read_buff[index])
        {
            error = 1u;
            break;
        }
        index++;
        size--;
    }

    return error;
}
/******************************************************************************/
void transfer_complete_handler(uint32_t status)
{
    uint32_t isr_err;

    if (ERROR_INTERRUPT & status)
    {
        isr_err = status >> 16u;
    }
    else if (TRANSFER_COMPLETE & status)
    {
        isr_err = 0u;
    }
    /* command queue task completed */
    else if (CQ_INTERRUPT & status)
    {
        isr_err = 0u;
    }
    else
    {

    }

}

/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_1.
 *
 */

#include <stdio.h>
#include "mpfs_hal/mss_hal.h"
#include "drivers/mss_mmuart/mss_uart.h"
#include "../inc/pac1934.h"

extern void uart_tx_with_mutex (mss_uart_instance_t * this_uart,
                                uint64_t mutex_addr,
                                const uint8_t * pbuff);

extern uint64_t uart_lock;
uint8_t rx_size = 0;
uint8_t g_rx_buff[1] = {0};

/* Main function for the hart1(U54_1 processor).
 * Application code running on hart1 is placed here
 *
 * The hart1 goes into WFI. hart0 brings it out of WFI when it raises the first
 * Software interrupt to this hart.
 */
void u54_1(void)
{
    volatile uint32_t icount = 0U;

    /* Clear pending software interrupt in case there was any.
       Enable only the software interrupt so that the E51 core can bring this
       core out of WFI by raising a software interrupt. */
    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

    /* Put this hart into WFI. */
    do
    {
        __asm("wfi");
    }while(0 == (read_csr(mip) & MIP_MSIP));

    /* The hart is out of WFI, clear the SW interrupt. Here onwards application
     * can enable and use any interrupts as required */
    clear_soft_interrupt();

    __enable_irq();
    PLIC_init();

    if (0 == PAC1934_sensor_probe())
    {
        uart_tx_with_mutex (&g_mss_uart0_lo, (uint64_t)&uart_lock,
                            "\r\nPAC1934 sensor probe successful \r\n");
        PAC1934_drawVB();
        PAC1934_drawISense();
        uart_tx_with_mutex (&g_mss_uart0_lo, (uint64_t)&uart_lock,
                            "\r\nPress a key to refresh the sensor data \r\n");
    }
    else
    {
        uart_tx_with_mutex (&g_mss_uart0_lo, (uint64_t)&uart_lock,
                            "\r\nPAC1934 sensor probe failed \r\n");
    }

    while (1u)
    {
        rx_size = MSS_UART_get_rx(&g_mss_uart0_lo, g_rx_buff, sizeof(g_rx_buff));

        if(rx_size > 0)
        {
            switch (g_rx_buff[0u])
            {
                case 1:
                default:
                PAC1934_drawVB();
                PAC1934_drawISense();
                break;
            }
        }
    }

    /* never return */
}

/* hart1 Software interrupt handler */
void Software_h1_IRQHandler(void)
{
}

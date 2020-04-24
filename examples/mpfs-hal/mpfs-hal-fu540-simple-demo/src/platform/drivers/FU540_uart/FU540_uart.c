/*******************************************************************************
 * Copyright 2019 Microchip Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * FU540_uart.c
 * Uart integrated in the SiFive FU540
 *
 *
 * SVN $Revision: 11806 $
 * SVN $Date: 2019-07-25 12:34:03 +0530 (Thu, 25 Jul 2019) $
 */
#if (MPFS==0)

#include "FU540_uart.h"
#include "mpfs_hal/mss_plic.h"
#include "mpfs_hal/mss_util.h"
#include "hal/hal.h"
#include "inc/common.h"

uint64_t lock;

mss_FU540_uart_instance_t g_mss_FU540_uart0;
mss_FU540_uart_instance_t g_mss_FU540_uart1;

#define ASSERT(CHECK)

void
MSS_FU540_UART_init
(
    mss_FU540_uart_instance_t* this_uart,
    uint32_t baud_rate,
    uint8_t line_config
)
{
    static uint8_t uart0buffer[256];
    mss_init_mutex((uint64_t)&lock);

    /* The driver expects g_mss_uart0 and g_mss_uart1 to be the only
     * mss_uart_instance_t instances used to identify UART0 and UART1. */
    ASSERT((this_uart == &g_mss_uart0) || (this_uart == &g_mss_uart1));


    this_uart = &g_mss_FU540_uart0;

    this_uart->hw_reg = (FU540_UART *) BASE32_ADDR_FU540_UART0;

    this_uart->hw_reg->txctrl = ((0x01<<16) | 0x01); /* 0x01 => tx enable, (0x01<<18) => watermark = 1; */
    this_uart->hw_reg->rxctrl = ((0x01<<16) | 0x01);

    this_uart->tx_buffer      =  (uint8_t *)uart0buffer;          /*!< Pointer to transmit buffer. */
    this_uart->tx_buff_size   =  sizeof(uint32_t) * 256U;         /*!< Transmit buffer size. */

    __enable_irq();
}

/***************************************************************************//**
 * See FU540_uart.h for details of how to use this function.
 */
uint32_t
MSS_FU540_UART_get_rx
(
    mss_FU540_uart_instance_t * this_uart,
    uint8_t * rx_buff,
    uint32_t buff_size
)
{
    mss_take_mutex((uint64_t)&lock);
    uint32_t rx_size = 0U;
    uint32_t status = 0U;


    ASSERT(rx_buff != ((uint8_t *)0));
    ASSERT(buff_size > 0u);

    if((rx_buff != (uint8_t *)0) && (buff_size > 0u))
    {
        status = this_uart->hw_reg->rxdata;
        this_uart->status |= status;

        while(((status & RX_EMPTY_MASK) == 0u) && (rx_size < buff_size))
        {
            rx_buff[rx_size] = (uint8_t)status;
            ++rx_size;
            status = this_uart->hw_reg->rxdata;
            this_uart->status |= status;
        }
    }
    mss_release_mutex((uint64_t)&lock);
    return rx_size;
}

/***************************************************************************//**
 * See FU540_uart.h for details of how to use this function.
 */
void
MSS_FU540_UART_interrupt_tx
(
    mss_FU540_uart_instance_t * this_uart,
    const uint8_t * pbuff,
    uint32_t tx_size
)
{
    MSS_FU540_UART_int_tx ( this_uart, pbuff, tx_size );
}

/***************************************************************************//**
 * See mss_uart.h for details of how to use this function.
 */
void
MSS_FU540_UART_polled_tx
(
    mss_FU540_uart_instance_t * this_uart,
    const uint8_t * pbuff,
    uint32_t tx_size
)
{
    mss_take_mutex((uint64_t)&lock);
    uint32_t char_idx = 0U;
    uint32_t status;

    ASSERT((this_uart == &g_mss_uart0) || (this_uart == &g_mss_uart1));
    ASSERT(pbuff != ( (uint8_t *)0));
    ASSERT(tx_size > 0u);
    if(((this_uart == &g_mss_FU540_uart0) || (this_uart == &g_mss_FU540_uart1)) &&
        (pbuff != ((uint8_t *)0)) && (tx_size > 0u))
    {
         /* Remain in this loop until the entire input buffer
          * has been transferred to the UART.
          */
        do {
            /* Read the Status and update the sticky record */
            status = this_uart->hw_reg->txdata;
            this_uart->status = (uint32_t)(status & TX_FULL_MASK);

            /* Check if TX FIFO is empty. */
            if(status == 0)
            {
                this_uart->hw_reg->txdata = pbuff[char_idx++];

                /* Calculate the number of untransmitted bytes remaining. */
                tx_size -= (uint32_t)1U;
            }
        } while(tx_size);
    }
    mss_release_mutex((uint64_t)&lock);
}


/**
 * This tx function Transmits using an interrupt
 */
void
MSS_FU540_UART_int_tx
(
    mss_FU540_uart_instance_t * this_uart,
    const uint8_t * pbuff,
    uint32_t tx_size
)
{
    uint32_t char_idx = 0U;
    volatile uint32_t ix=0U;

    uint32_t status;

    ASSERT((this_uart == &g_mss_uart0) || (this_uart == &g_mss_uart1));
    ASSERT(pbuff != ( (uint8_t *)0));
    ASSERT(tx_size > 0u);

    if(((this_uart == &g_mss_FU540_uart0) || (this_uart == &g_mss_FU540_uart1)) &&
        (pbuff != ((uint8_t *)0)) && (tx_size > 0U))
    {
         /* Remain in this loop until the entire input buffer
          * has been transferred to the UART.
          */
        do {
            /*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
             fixme: had to add in to get print to work properly when using interrupt
             do not know why, needs to be investigated  */
            volatile uint32_t i, dummy_h1 = 0U;
            uint32_t num_loops = 100U;  /* if this is changed to 10, print does not work correctly */

            for(i = 0U; i < num_loops; i++)
            {
                dummy_h1 = i;
            }
            /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< */
            /* Read the Status and update the sticky record */
            status = this_uart->hw_reg->txdata;
            this_uart->status = (uint32_t)(status & TX_FULL_MASK);

            /* Check if TX FIFO is empty. */
            while(this_uart->hw_reg->txdata & TX_FULL_MASK)
                ix++;
            {
                this_uart->tx_buffer[0] = pbuff[char_idx++];
                this_uart->hw_reg->ie   = 0x01;         /* prepare interrupt */
                /* Calculate the number of non transmitted bytes remaining. */
                tx_size -= (uint32_t)1;
            }
        } while(tx_size);
    }

}

/***************************************************************************//**
 * Tx interrupt
 */
uint8_t USART0_plic_4_IRQHandler(void)
{
    volatile uint32_t ix = 0U;
    g_mss_FU540_uart0.hw_reg->ie = 0x00U;   /* disable UART interrupt */

    /* Check if TX FIFO is empty. */
    while(g_mss_FU540_uart0.hw_reg->txdata & TX_FULL_MASK)
      ix++;
    g_mss_FU540_uart0.hw_reg->txdata = g_mss_FU540_uart0.tx_buffer[0];

    return EXT_IRQ_KEEP_ENABLED;
}


#endif

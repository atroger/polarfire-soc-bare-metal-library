/***********************************************************************************
 * (c) Copyright 2018 Microsemi-PRO Embedded Systems Solutions. All rights reserved.
 *
 * code running on U54 hart 3
 *
 * SVN $Revision$
 * SVN $Date$
 */
#include "../mpfs_hal/mss_util.h"
#include "../mpfs_hal/mss_clint.h"
#include "../common/common.h"
#ifndef SIFIVE_HIFIVE_UNLEASHED
#include "../drivers/mss_uart/mss_uart.h"
#else
#include "../drivers/FU540_uart/FU540_uart.h"
#endif

volatile uint64_t dummy_h3 = 0U;
volatile uint32_t count_sw_ints_h3 = 0U;
volatile uint32_t loop_count_h3 = 0U;

/**
 *
 */
void u54_3(void)
{
	volatile uint64_t * dummy_addr = (volatile uint64_t *)DDR_BASE;
	uint32_t hartid = read_csr(mhartid);
	int8_t info_string[100];
    volatile uint32_t   i = 0U;
    volatile uint32_t   test = 0U;

    clear_soft_interrupt();
    while(( test & MIP_MSIP) == 0U)
    {
        __asm("nop");
        __asm("nop");
        __asm("wfi");          /* wait for interprocess from hart0 to wake up */
        test = read_csr(mip);
    }

	while(1U)
	{
		i++;
		if(i == 0x100000U)
		{
			i = 0U;
			loop_count_h3++;
			sprintf(info_string,"Hart %d\n\r", hartid);
			mss_take_mutex((uint64_t)&uart_lock);
			MSS_UART_polled_tx(&g_mss_uart0_lo, info_string,strlen(info_string));
			mss_release_mutex((uint64_t)&uart_lock);
		}

	}
};



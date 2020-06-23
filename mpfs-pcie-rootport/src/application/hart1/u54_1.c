/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 */
/*******************************************************************************
 *
 * Code running on U54 first hart
 *
 */
#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"

/* Main function for the HART1(U54_1 processor).
 * Application code running on HART1 is placed here
 *
 * The HART1 goes into WFI and other HART's brings it out of WFI when it raises
 * the first Software interrupt to this HART
 */
void u54_1(void)
{
    uint64_t hartid = read_csr(mhartid);
    volatile uint64_t icount = 0U;

    /* Clear pending software interrupt in case there was any.
    Enable only the software interrupt so that other core can bring this core
    out of WFI by raising a software interrupt.
    Note that any other interrupt can also be used to bring CPU out of WFI */
    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);

    /*Put this hart into WFI.*/
    do
    {
        __asm("wfi");
    }while(0 == (read_csr(mip) & MIP_MSIP));

    /*The hart is out of WFI, clear the SW interrupt. Hear onwards Application
     * can enable and use any interrupts as required*/
    clear_soft_interrupt();

    __enable_irq();

    while (1U)
    {
        icount++;
        if (0x100000U == icount)
        {
            icount = 0U;
        }
    }
    /* never return */
}

/* HART1 Software interrupt handler */
void Software_h1_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
}

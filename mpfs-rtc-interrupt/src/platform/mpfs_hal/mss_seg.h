/*******************************************************************************
 * Copyright 2019-2020 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT 
 *
 * MPFS HAL Embedded Software
 *
 */

/***************************************************************************
 *
 * @file mss_seg.h
 * @author Microchip FPGA Embedded Systems Solutions
 * @brief segmentation block defines
 *
 * These blocks allow the DDR memory to be allocated to cached, non-cached
 * regions and trace depending on the amount of DDR memory physically connected.
 * Conceptually an address offset is added/subtracted from the DDR address
 * provided by the Core Complex to point at a base address in the DDR memory.
 *
 * The AXI bus simply passes through the segmentation block, and the address
 * is modified.
 *
 * There are two segmentation blocks, they are grouped into the same address
 * ranges as the MPU blocks. Each one has seven 32-segmentation registers, but
 * only two in SEG0 and five in SEG1 are actually implemented.
 *
 */

#ifndef MPFS_SEG_H
#define MPFS_SEG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

    struct {
        volatile int32_t    offset : 15;
        volatile int32_t    rsrvd  : 16;
        volatile int32_t    locked : 1;
    } CFG[6];

    uint32_t fill[64-6];

} seg_t;

#define SEG ((seg_t*) 0x20005d00)

#ifdef __cplusplus
}
#endif

#endif /*MPFS_SEG_H*/

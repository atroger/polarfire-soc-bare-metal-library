/*******************************************************************************
 * (c) Copyright 20019 Microchip-PRO Embedded Systems Solutions.
 * All rights reserved.
 * 
 *
 *  
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdint.h>

typedef enum COMMAND_TYPE_
{
    CLEAR_COMMANDS                  = 0x00,       /*!< 0 default behavior */
    START_HART1_U_MODE              = 0x01,       /*!< 1 u mode */
    START_HART2_S_MODE              = 0x02,       /*!< 2 s mode */
}   COMMAND_TYPE;


typedef enum MEM_TYPE_
{
    E51_DDR_START_NON_CACHE         = 0x00,       /*!< 0 non cached */
    E51_DDR_START_CACHE             = 0x01,       /*!< cached */
    U54_1_DDR_START_NON_CACHE       = 0x02,       /*!< 0 non cached */
    U54_1_DDR_START_CACHE           = 0x03        /*!< cached */
}   MEM_TYPE;


/**
 * extern variables
 */

/**
 * functions
 */
void e51(void);
void u54_1(void);
void u54_2(void);
void u54_3(void);
void u54_4(void);
void jump_to_application( MEM_TYPE mem_area);

#endif /* COMMON_H_ */

/*
 * opt3001.h
 *
 *  Created on: Oct 20, 2020
 *      Author: Joe Krachey
 */

#ifndef LM75_H_
#define LM75_H_

#include <stdint.h>
#include <stdbool.h>
#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"

/* ADD CODE */
#define LM75_SUBORDINATE_ADDR                 0x4F
#define LTR_SUBORDINATE_ADDR                  0x29


/* ADD CODE */
#define LM75_TEMP_REG						  0x00
#define ALS_DATA_CH1_LOW                      0x88
#define ALS_DATA_CH1_HIGH                     0x89 
#define ALS_DATA_CH0_LOW                      0x8A
#define ALS_DATA_CH0_HIGH                     0x8B
/**
 *
 *
 *
 */
cy_rslt_t LM75_init(void);

/** Read the temperature from LM75
 *
 * @param
 *
 */
float LM75_get_temp(void);

int LTR_get_data(void);

#endif /* LM75_H_ */

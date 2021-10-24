/*
 * common_timer.h
 *
 *  Created on: Oct 24, 2021
 *      Author: muaddib
 */

#ifndef INC_HARDWARE_COMMON_TIMER_H_
#define INC_HARDWARE_COMMON_TIMER_H_

#include "global_inc.h"

#define CTMR_US_PRESC           (1000000)
#define CTMR_MS_PRESC           (1000)

volatile uint32_t * ctmr_get_timer_prt(void);
uint32_t ctmr_get(void);
uint32_t ctmr_start_timer(uint32_t timeout);
bool ctmr_is_timer_expired(uint32_t timer);
void ctmr_delay(uint32_t delay);
void ctmr_init_ms(uint32_t system_clk);

#endif /* INC_HARDWARE_COMMON_TIMER_H_ */

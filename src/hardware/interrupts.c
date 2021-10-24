/*
 * interrupts.c
 *
 *  Created on: Oct 24, 2021
 *      Author: muaddib
 */
#include "mcu.h"
#include "common_timer.h"
#include "interrupts.h"

#define NVIC_PRIORITYGROUP_0         0x00000007U /*!< 0 bits for pre-emption priority
                                                      4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         0x00000006U /*!< 1 bits for pre-emption priority
                                                      3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         0x00000005U /*!< 2 bits for pre-emption priority
                                                      2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         0x00000004U /*!< 3 bits for pre-emption priority
                                                      1 bits for subpriority */
#define NVIC_PRIORITYGROUP_4         0x00000003U /*!< 4 bits for pre-emption priority
                                                      0 bits for subpriority */

#define PG4_LOWEST_PRIORITY (255)

static volatile uint32_t * s_tick_timer_ptr;


void interrupts_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    //timer 2 init interrupt
    NVIC_SetPriority(TIM2_IRQn, PG4_LOWEST_PRIORITY);
    NVIC_EnableIRQ(TIM2_IRQn);
    s_tick_timer_ptr = ctmr_get_timer_prt();
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_DIER_UIE;
    (*s_tick_timer_ptr)++;
}

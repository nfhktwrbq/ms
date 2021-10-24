/*
 * common_timer.c
 *
 *  Created on: Oct 24, 2021
 *      Author: muaddib
 */

#include "common_timer.h"

static volatile uint32_t s_timer = 0;

void ctmr_init_ms(uint32_t system_clk)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    (void)RCC->APB1ENR;

    TIM2->PSC = system_clk / 1000000 - 1;
    TIM2->ARR = 1000 - 1;
    TIM2->EGR = TIM_EGR_UG;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
}

volatile uint32_t * ctmr_get_timer_prt(void)
{
    return &s_timer;
}

uint32_t ctmr_get(void)
{
    return s_timer;
}

uint32_t ctmr_start_timer(uint32_t timeout)
{
    return timeout + s_timer;
}

bool ctmr_is_timer_expired(uint32_t timer)
{
    return s_timer > timer;
}

void ctmr_delay(uint32_t delay)
{
    uint16_t timeout = s_timer + delay;
    while (s_timer < timeout)
    {
    }
}

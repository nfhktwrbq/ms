/*
 * interrupts.c
 *
 *  Created on: Oct 24, 2021
 *      Author: muaddib
 */
#include "mcu.h"
#include "common_timer.h"
#include "display_spi.h"
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
#define PG4_MEDIUM_PRIORITY (128)

static volatile uint32_t * s_tick_timer_ptr;


void interrupts_init(void)
{
    NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

    //timer 2 init interrupt
    NVIC_SetPriority(TIM2_IRQn, PG4_LOWEST_PRIORITY);
    NVIC_EnableIRQ(TIM2_IRQn);
    s_tick_timer_ptr = ctmr_get_timer_prt();

    NVIC_SetPriority(DMA1_Channel5_IRQn, PG4_LOWEST_PRIORITY);
    NVIC_EnableIRQ(DMA1_Channel5_IRQn);

    NVIC_SetPriority(TIM3_IRQn, PG4_MEDIUM_PRIORITY);
    NVIC_EnableIRQ(TIM3_IRQn);
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    (*s_tick_timer_ptr)++;
}

void DMA1_Channel5_IRQHandler(void)
{
//    if (DMA1->ISR & DMA_ISR_TCIF5)
//    {
//        DMA1->IFCR = DMA_IFCR_CHTIF5 | DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5;
//        DMA1_Channel5->CCR &= ~DMA_CCR_TCIE;
//    }
}


void TIM3_IRQHandler(void)
{
    if (is_d_spi_ready())
    {
        D_SPI->CR2 &= ~SPI_CR2_TXDMAEN;
        DMA1_Channel5->CCR &= ~DMA_CCR_EN;
        display_descr_s * d_descriptor = d_spi_descriptor_get();

        if (d_spi_is_last_data())
        {
            TIM3->DIER &= ~TIM_DIER_UIE;
            d_spi_cs_high();
            return;
        }

        if (d_descriptor->current_part > 0)
        {
            d_spi_cs_strobe();
        }
        //Clears all interrupts flags
        //DMA1->IFCR = DMA_IFCR_CHTIF5 | DMA_IFCR_CGIF5 | DMA_IFCR_CTCIF5 | DMA_IFCR_CTEIF5;
        DMA1_Channel5->CNDTR = d_descriptor->part_len;
        DMA1_Channel5->CMAR = (uint32_t)d_spi_get_data_part();
        DMA1_Channel5->CPAR = (uint32_t)(&SPI2->DR);
        DMA1_Channel5->CCR |= DMA_CCR_EN /*| DMA_CCR_TCIE*/;

        D_SPI->CR2 |= SPI_CR2_TXDMAEN;
        d_descriptor->current_part += 1;

    }

    TIM3->SR &= ~TIM_SR_UIF;
}

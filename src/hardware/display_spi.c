/*
 * spi.c
 *
 *  Created on: Oct 30, 2021
 *      Author: muaddib
 */
#include "global_inc.h"
#include "global_def.h"
#include "mcu.h"
#include "spi.h"
#include "display_spi.h"
#include "misc.h"

#define _SPI_CLK_DIV    (8)
#define _SPI_

static display_descr_s display_descr;

void d_spi_cs_strobe(void)
{
    GPIOB->BSRR |= GPIO_BSRR_BS12;
    GPIOB->BSRR |= GPIO_BSRR_BR12;
}

void d_spi_cs_high(void)
{
    GPIOB->BSRR |= GPIO_BSRR_BS12;
}

void d_spi_cs_low(void)
{
    GPIOB->BSRR |= GPIO_BSRR_BR12;
}

void d_spi_init(void)
{
    //Enable clock on SPI2
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
    (void) RCC->APB1ENR;

    //DMA1EN: DMA1 clock enable
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    (void) RCC->AHBENR;

    //AFIOEN: Alternate function IO clock enable
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    (void) RCC->APB2ENR;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
    (void) RCC->APB2ENR;

    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    (void) RCC->APB1ENR;

    TIM3->PSC = 0;
    TIM3->ARR = ((CONTINUOUS_DATA_LEN) * BITS_PER_BYTE * _SPI_CLK_DIV);
    TIM3->EGR = TIM_EGR_UG;
    TIM3->CR1 |= TIM_CR1_CEN;

    //SPI2_SS SPI2_CLK SPI2_MOSI to 01: Output mode, max speed 10 MHz.
    //SPI2_SS SPI2_CLK SPI2_MOSI to 10: Alternate function output Push-pull
    GPIOB->CRH &= ~(GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE15 |
                    GPIO_CRH_CNF12 | GPIO_CRH_CNF13 | GPIO_CRH_CNF15);
    GPIOB->CRH |= GPIO_CRH_MODE12 | GPIO_CRH_MODE13 | GPIO_CRH_MODE15 |
                  /*GPIO_CRH_CNF12_1==0 | */GPIO_CRH_CNF13_1 | GPIO_CRH_CNF15_1;

    GPIOB->BSRR |= GPIO_BSRR_BR12;

    //SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE - Output enabled (transmit-only mode)
    //CRCEN Hardware CRC calculation enable 0: CRC calculation disabled
    //CRCNEXT: CRC transfer next 0: Data phase (no CRC phase)
    //DFF: Data frame format 0: 8-bit data frame format is selected for transmission/reception
    //RXONLY: Receive only 0: Full duplex (Transmit and receive)
    //SSM: Software slave management 0: Software slave management disabled
    //SSI: Internal slave select  This bit has an effect only when the SSM bit is set.
    //LSBFIRST: Frame format 0: MSB transmitted first
    //SPE: SPI enable 1: Peripheral enabled
    //BR[2:0]: Baud rate control 101: fPCLK/64
    //MSTR: Master selection 1: Master configuration
    //CPOL: Clock polarity 0: CK to 0 when idle
    //CPHA: Clock phase 0: The first clock transition is the first data capture edge
    uint32_t baudrate = find_bit_position(_SPI_CLK_DIV) - 1;
    if (baudrate < 0)
    {
        D_SPI->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2| SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    }
    else
    {
        D_SPI->CR1 = SPI_CR1_BIDIMODE | SPI_CR1_BIDIOE | (baudrate << SPI_CR1_BR_Pos) | SPI_CR1_MSTR | SPI_CR1_SSI | SPI_CR1_SSM;
    }

    //TXEIE: Tx buffer empty interrupt enable 0: TXE interrupt masked
    //RXNEIE: RX buffer not empty interrupt enable 0: RXNE interrupt masked
    //ERRIE: Error interrupt enable 0: Error interrupt is masked
    //SSOE: SS output enable 1: SS output is enabled in master mode and when the cell is enabled. The cell cannot work in a multimaster environment.
    //TXDMAEN: Tx buffer DMA enable 1: Tx buffer DMA enabled
    //RXDMAEN: Rx buffer DMA enable 0: Rx buffer DMA disabled
    D_SPI->CR2 = SPI_CR2_SSOE /*| SPI_CR2_TXDMAEN*/;

    D_SPI->I2SCFGR &= ~SPI_I2SCFGR_I2SMOD;


    //MEM2MEM: Memory to memory mode 0: Memory to memory mode disabled
    //PL[1:0]: Channel priority level 01: Medium
    //MSIZE[1:0]: Memory size 00: 8-bits
    //PSIZE[1:0]: Peripheral size 00: 8-bits
    //MINC: Memory increment mode 1: Memory increment mode enabled
    //PINC: Peripheral increment mode 0: Peripheral increment mode disabled
    //CIRC: Circular mode 0: Circular mode disabled
    //DIR: Data transfer direction 1: Read from memory
    //TEIE: Transfer error interrupt enable 0: TE interrupt disabled
    //HTIE: Half transfer interrupt enable 0: HT interrupt disabled
    //TCIE: Transfer complete interrupt enable 0: TC interrupt disabled
    //EN: Channel enable 1: Channel enabled
    DMA1_Channel5->CCR = DMA_CCR_MINC | DMA_CCR_DIR;

    //NDT[15:0]: Number of data to transfer
    DMA1_Channel5->CNDTR = DATA_TX_SIZE;

    //PA[31:0]: Peripheral address
    DMA1_Channel5->CPAR = (uint32_t)(&SPI2->DR);

    D_SPI->CR1 |= SPI_CR1_SPE;

    display_descr.part_len = CONTINUOUS_DATA_LEN;
}



void d_spi_send(void)
{
    //wait if enabled  previous data transfering
    while (TIM3->DIER & TIM_DIER_UIE)
    {
    }

    display_descr.current_part = 0;
    d_spi_cs_low();

    //start time interrupt. All logic in TIM3_IRQHandler
    TIM3->CNT = 0;
    TIM3->DIER |= TIM_DIER_UIE;
}

bool is_d_spi_ready(void)
{
    return is_spi_ready(D_SPI);
}

display_descr_s * d_spi_descriptor_get(void)
{
    display_descr.is_busy = (bool)(TIM3->DIER & TIM_DIER_UIE);
    return &display_descr;
}

bool d_spi_is_last_data(void)
{
    return display_descr.part_len * display_descr.current_part >= display_descr.data_len;
}

uint8_t * d_spi_get_data_part(void)
{
    return &display_descr.data[display_descr.part_len * display_descr.current_part];
}

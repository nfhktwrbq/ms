/*
 * spi.c
 *
 *  Created on: Oct 30, 2021
 *      Author: muaddib
 */
#include "spi.h"
#include "mcu.h"

bool is_spi_ready(SPI_TypeDef * spi)
{
    return (spi->SR & SPI_SR_BSY) == 0 && (spi->SR & SPI_SR_TXE);
}

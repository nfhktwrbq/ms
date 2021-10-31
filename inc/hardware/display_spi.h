/*
 * spi.h
 *
 *  Created on: Oct 30, 2021
 *      Author: muaddib
 */

#ifndef INC_HARDWARE_SPI_H_
#define INC_HARDWARE_SPI_H_

#include "global_inc.h"

#define D_SPI               SPI2
#define MATRIX_QTY          (4)
#define BYTES_PER_MATRIX    (8)
#define DATA_TX_SIZE        (BYTES_PER_MATRIX * MATRIX_QTY)
#define CONTINUOUS_DATA_LEN (2)
#define MATRIX_HEIGHT       (8)
#define MAX_DISPALY_CMD_LEN (128)

typedef struct
{
    uint8_t data[MAX_DISPALY_CMD_LEN];
    uint8_t data_len;
    uint8_t part_len;
    uint8_t current_part;
    bool is_busy;
} display_descr_s;

void d_spi_init(void);
void d_spi_send(void);
void d_spi_cs_strobe(void);
bool is_d_spi_ready(void);
display_descr_s * d_spi_descriptor_get(void);
void d_spi_cs_low(void);
void d_spi_cs_high(void);
bool d_spi_is_last_data(void);
uint8_t * d_spi_get_data_part(void);

#endif /* INC_HARDWARE_SPI_H_ */

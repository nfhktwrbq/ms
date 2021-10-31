/*
 * display.c
 *
 *  Created on: Oct 30, 2021
 *      Author: muaddib
 */
#include "display.h"
#include "display_spi.h"
#include "cmsis_os.h"
#include "common_timer.h"

#define _WAIT_SEND_TO_MS    (100)
#define DECODE_MODE_REG     (0x09)
#define INTENSITY_REG       (0x0A)
#define SCAN_LIMIT_REG      (0x0B)
#define SHUTDOWN_REG        (0x0C)
#define TEST_REG            (0x0F)

#define DECODE_MODE_NO_DECODE       (0x00)
#define DECODE_MODE_CODE_B_FOR_0    (0x01)
#define DECODE_MODE_CODE_B_FOR_3_0  (0x0f)
#define DECODE_MODE_CODE_B_FOR_7_0  (0xff)

static uint32_t _display_buffer[8];


static void _display_add_command(display_descr_s * descr, uint8_t address, uint8_t command, uint8_t * data_pos)
{
    descr->data[*data_pos] = address;
    *data_pos += 1;
    descr->data[*data_pos] = command;
    *data_pos += 1;
    descr->data_len = *data_pos;
}

void display_add_digit(display_descr_s * descr, uint8_t pos, uint8_t value, uint8_t * data_pos)
{
    if (pos > 8)
    {
        return;
    }
    _display_add_command(descr, pos, value, data_pos);
}

static display_descr_s *  _display_get_descriptor(void)
{
    uint32_t timer = ctmr_start_timer(_WAIT_SEND_TO_MS);

    display_descr_s * d_descr = d_spi_descriptor_get();
    while (d_descr->is_busy && !ctmr_is_timer_expired(timer))
    {
        osDelay(1);
        d_descr = d_spi_descriptor_get();
    }

    if (ctmr_is_timer_expired(timer))
    {
        DEBUG_PRINT("TO occurred for display init");
        return NULL;
    }

    return d_descr;
}

void display_init(void)
{
    uint8_t data_pos = 0;
    d_spi_init();

    display_descr_s * d_descr = _display_get_descriptor();

    if (d_descr)
    {
        _display_add_command(d_descr, DECODE_MODE_REG, DECODE_MODE_NO_DECODE, &data_pos);
        _display_add_command(d_descr, INTENSITY_REG, 1, &data_pos);
        _display_add_command(d_descr, SCAN_LIMIT_REG, 7, &data_pos);
        _display_add_command(d_descr, SHUTDOWN_REG, 1, &data_pos);
        _display_add_command(d_descr, TEST_REG, 0, &data_pos);
        _display_add_command(d_descr, TEST_REG, 0, &data_pos);
        _display_add_command(d_descr, TEST_REG, 0, &data_pos);
        _display_add_command(d_descr, TEST_REG, 0, &data_pos);
        d_spi_send();
    }
}

void display_set_intensity(uint8_t intensity)
{
    if (intensity > 16)
    {
        intensity = 16;
    }

    display_descr_s * d_descr = _display_get_descriptor();
    uint8_t data_pos = 0;

    if (d_descr)
    {
        for (uint8_t i = 0; i < MATRIX_QTY; i++)
        {
            _display_add_command(d_descr, INTENSITY_REG, intensity, &data_pos);
        }

        for(uint8_t i = 0; i < BYTES_PER_MATRIX; i++)
        {
            for(int8_t j = 3; j >= 0; j--)
            {
                display_add_digit(d_descr, i + 1,(uint8_t)((_display_buffer[i] >> 8 * j) & 0x000000ffUL), &data_pos);
            }
        }
        d_spi_send();
    }
}

void display_show(void)
{
    uint8_t data_pos = 0;
    display_descr_s * d_descr = _display_get_descriptor();

    if (d_descr)
    {
        for(uint8_t i = 0; i < MATRIX_HEIGHT; i++)
        {
            for(int8_t j = 3; j >= 0; j--)
            {
                display_add_digit(d_descr, i + 1,(uint8_t)((_display_buffer[i] >> 8 * j) & 0x000000ffUL), &data_pos);
            }
        }
        d_spi_send();
    }
}

void display_task(void * parameter)
{
    display_init();
    _display_buffer[0] = 0xffff01ff;
    _display_buffer[1] = 0x7f7f017f;
    _display_buffer[2] = 0x3f3f013f;
    _display_buffer[3] = 0x1f1f011f;
    _display_buffer[4] = 0x0f0f010f;
    _display_buffer[5] = 0x07070107;
    _display_buffer[6] = 0x03030103;
    _display_buffer[7] = 0x01010101;
    while (1)
    {
        osDelay(1000);
        uint32_t tmp = _display_buffer[0];
        for (uint8_t i = 0; i < 7; i++)
        {
            _display_buffer[i] = _display_buffer[i+1];
        }
        _display_buffer[7] = tmp;
        DEBUG_PRINT("Send display data");
        display_show();
    }
}


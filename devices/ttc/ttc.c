/*
 * ttc.c
 *
 * Copyright (C) 2020, SpaceLab.
 *
 * This file is part of EPS 2.0.
 *
 * EPS 2.0 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * EPS 2.0 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with EPS 2.0. If not, see <http://www.gnu.org/licenses/>.
 *
 */

/**
 * \brief TTC device implementation.
 *
 * \author Alvaro Aranibar Roque <aranibar.a@pucp.edu.pe>
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 * \author Augusto Cezar Boldori Vassoler <augustovassoler@gmail.com>
 *
 * \version 0.2.29
 *
 * \date 22/04/2021
 *
 * \addtogroup ttc
 * \{
 */

#include <stdbool.h>

#include <system/sys_log/sys_log.h>
#include <app/structs/eps2_data.h>

#include "ttc.h"

#define TTC_CRC8_INITIAL_VALUE          0       /**< CRC8-CCITT initial value. */
#define TTC_CRC8_POLYNOMIAL             0x07    /**< CRC8-CCITT polynomial. */

ttc_config_t ttc_config = {0};

int ttc_init(void)
{
    sys_log_print_event_from_module(SYS_LOG_INFO, TTC_MODULE_NAME, "Initializing TTC device.");
    sys_log_new_line();

    /* TTC configuration */
    ttc_config.uart_port   = UART_PORT_0;
    ttc_config.uart_config = (uart_interrupt_config_t){.baudrate=115200, .data_bits=8, .parity=UART_NO_PARITY, .stop_bits=UART_ONE_STOP_BIT};

    if (uart_interrupt_init(ttc_config.uart_port, ttc_config.uart_config) != 0)
    {
        sys_log_print_event_from_module(SYS_LOG_ERROR, TTC_MODULE_NAME, "Error during the initialization!");
        sys_log_new_line();
        
        return -1;
    }

    if (uart_interrupt_enable(ttc_config.uart_port) != 0)
    {
        sys_log_print_event_from_module(SYS_LOG_ERROR, TTC_MODULE_NAME, "Error during the initialization!");
        sys_log_new_line();
        
        return -1;
    }

    return 0;
}

int ttc_decode(uint8_t *adr, uint32_t *val, uint8_t *cmd) 
{
    int err = 0;
    uint8_t buf[UART_RX_BUFFER_MAX_SIZE] = {0};
    uint16_t received_size = 0;
    uint8_t wr_single_param_size = 0;

    if (uart_interrupt_read(ttc_config.uart_port, buf, &received_size, cmd, &wr_single_param_size) == 0)
    {
	    if(ttc_check_crc(buf, received_size - 1U, buf[received_size - 1U]) == true)
	    {
		    switch (*cmd)
            {
                case REQ_SINGLE_PARAM:
                    *adr = buf[1];
                    *val = 0;
                    break;
                case WR_SINGLE_PARAM:
                    *adr = buf[1];
                    switch (wr_single_param_size)
                    {
                        case 1:
                            *val = (uint32_t) buf[3];
                            break;
                        case 2:
                            *val =  ((uint32_t)buf[3] << 8) |
                                    ((uint32_t)buf[4]);
                            break;
                        case 4:
                            *val =  ((uint32_t)buf[3] << 24) |
                                    ((uint32_t)buf[4] << 16) |
                                    ((uint32_t)buf[5] << 8)  |
                                    ((uint32_t)buf[6]);
                            break;
                        default:
                            err = -1;
                            sys_log_print_event_from_module(SYS_LOG_ERROR, TTC_MODULE_NAME, "Invalid payload size!");
                            sys_log_new_line();
                            break;
                    }
                    break;
                case REQ_FULL_TLM:
                case REQ_BEACON_TLM:
                case ANOMALY:
                    *adr = 0;
                    *val = 0;
                    break;
                default:
                    sys_log_print_event_from_module(SYS_LOG_ERROR, TTC_MODULE_NAME, "Invalid command received (CMD)!");
                    sys_log_new_line();
                    err = -1;
                    break;
            }
	    }
	    else 
	    {
		    sys_log_print_event_from_module(SYS_LOG_ERROR, TTC_MODULE_NAME, "Invalid command received (CRC)!");
            sys_log_new_line();
		
		    err = -1;
	    }
    }
    else
    {
        err = -1;
    }

    return err;
}

int ttc_wr_id_buffer_out(uint8_t adr, uint32_t val)
{
    uint8_t buf[7] = {0};               // [Echo ID] + [Payload size] + [N Data Bytes] + [CRC]
    uint8_t size = 0;
    buf[0] = adr;
    if (eps_buffer_get_size(adr, &size) == 0)
    {
        buf[1] = size;
        switch (buf[1])
        {
            case 1:
                buf[2] =  val        & 0xFF;
                break;
            case 2:
                buf[2] = (val >> 8)  & 0xFF;
                buf[3] =  val        & 0xFF;
                break;
            case 4:
                buf[2] = (val >> 24) & 0xFF;
                buf[3] = (val >> 16) & 0xFF;
                buf[4] = (val >> 8)  & 0xFF;
                buf[5] =  val        & 0xFF;
                break;
            default:
                break;
        }
        buf[size+2] = ttc_crc8(buf, size+2);
    }
    return uart_interrupt_write(ttc_config.uart_port, buf, size+3);
}

int ttc_wr_packet_buffer_out(uint8_t cmd)
{
    uint8_t buf[UART_TX_BUFFER_MAX_SIZE] = {0};
    uint8_t id = 0;
    uint8_t j = 0;
    uint32_t val = 0;
    uint8_t size = 0;
    switch (cmd)
    {
        case REQ_FULL_TLM:
            for (id=0; id<EPS2_PARAM_MAX_COUNT; id++) { 
                if (eps_buffer_read(id, &val) != 0) {
                    val = 0XFFFFFFFF; // If there is an error, array is still filled with an error code to avoid shrinking.
                    sys_log_print_event_from_module(SYS_LOG_WARNING, TTC_MODULE_NAME, "Full_tlm Read fail. Padding with FF.");
                }
                
                if (eps_buffer_get_size(id, &size) == 0) {
                    switch (size) {
                        case 1: buf[j]   = val & 0XFF; break;
                        case 2: buf[j]   = (val >> 8)  & 0xFF; 
                                buf[j+1] = val & 0xFF; break;
                        case 4: buf[j]   = (val >> 24) & 0xFF; 
                                buf[j+1] = (val >> 16) & 0xFF; 
                                buf[j+2] = (val >> 8)  & 0xFF; 
                                buf[j+3] = val & 0xFF; break;
                    }
                    j += size;
                    val = 0;
                    size = 0;
                }
            }
            buf[j] = ttc_crc8(buf, j);
            break;
        case REQ_BEACON_TLM:
            for (id=0; id<EPS_BEACON_NUMBER_PARAM; id++)
            {
                if (eps_buffer_read(eps_param_id_beacon[id], &val) != 0) 
                {
                    val = 0XFFFFFFFF; // If there is an error, array is still filled with an error code to avoid shrinking.
                    sys_log_print_event_from_module(SYS_LOG_WARNING, TTC_MODULE_NAME, "Beacon_tlm read fail. Padding with FF.");
                }
                if ( eps_buffer_get_size(eps_param_id_beacon[id], &size) == 0)
                {
                    switch (size)
                    {
                        case 1:
                            buf[j]   =  val        & 0XFF;
                            break;
                        case 2:
                            buf[j]   = (val >> 8)  & 0xFF;
                            buf[j+1] =  val        & 0xFF;
                            break;
                        case 4:
                            buf[j]   = (val >> 24) & 0xFF;
                            buf[j+1] = (val >> 16) & 0xFF;
                            buf[j+2] = (val >> 8)  & 0xFF;
                            buf[j+3] =  val        & 0xFF;
                            break;
                        default:
                            break;
                    }
                    j = j+size;
                    val = 0;
                    size = 0;
                }
            }
            buf [j] = ttc_crc8(buf, j);
            break;
        default: 
            break;
    }
    return uart_interrupt_write(ttc_config.uart_port, buf, j+1);
}

int ttc_wr_ack_buffer_out(int err)
{
    uint8_t buf[2] = {0x00, 0x00};      // [ACK] | [Precalculated CRC8]
    if (err == -1)                      // [ERR] | [Precalculated CRC8]
    {
        buf[0] = 0xFF;
        buf[1] = 0xF3;
    }
    return uart_interrupt_write(ttc_config.uart_port, buf, 2);
}

int ttc_answer_long(uint8_t *buf, uint8_t len) 
{

    if (uart_interrupt_write(UART_PORT_0, buf, len) != 0)
    {
        return -1;
    }

    return 0;
}

uint8_t ttc_crc8(uint8_t *data, uint8_t len)
{
    uint8_t crc = TTC_CRC8_INITIAL_VALUE;

    while(len--)
    {
        crc ^= *data++;

        uint8_t j = 0;
        for (j=0; j<8; j++)
        {
            crc = (crc << 1) ^ ((crc & 0x80)? TTC_CRC8_POLYNOMIAL : 0);
        }

        crc &= 0xFF;
    }
    
    return crc;
}

bool ttc_check_crc(uint8_t *data, uint8_t len, uint8_t crc)
{
    if (crc != ttc_crc8(data, len))
    {
        return false;
    }

    return true;
}

/** \} End of ttc group */



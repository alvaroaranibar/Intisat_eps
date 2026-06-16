/*
 * ttc.h
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
 * \brief TTC device definition.
 *
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 * \author Augusto Cezar Boldori Vassoler <augustovassoler@gmail.com>
 *
 * \version 0.2.6
 *
 * \date 22/04/2020
 *
 * \defgroup ttc TTC
 * \ingroup devices
 * \{
 */

#ifndef TTC_H_
#define TTC_H_

#include <stdbool.h>

#include <drivers/uart_interrupt/uart_interrupt.h>

#include <devices/obdh/commands.h>

#define TTC_MODULE_NAME         "TTC"

/**
 * \brief TTC configuration type.
 */
typedef struct
{
    uart_interrupt_port_t uart_port;      		/**< UART port number. */
    uart_interrupt_config_t uart_config;		/**< UART port configuration. */
} ttc_config_t;

/**
 * \brief Initialization routine of the TTC device.
 *
 * \return The status/error code.
 */
int ttc_init(void);

/**
 * \brief Decodes a command from the TTC.
 *
 * \param[out] adr is the decoded register address.
 *
 * \param[out] val is the decoded register data.
 *
 * \param[out] cmd is the decoded command resquested.
 *
 * \return The status/error code.
 */
int ttc_decode(uint8_t *adr, uint32_t *val, uint8_t *cmd);

/**
 * \brief Writes a single parameter of the memory map to the output buffer.
 *
 * \param[in] adr is the register address requested.
 *
 * \param[in] val is the value to be read by the OBC/UHF
 *
 * \param[in] cmd is the OBC/UHF command
 *
 * \return The status/error code.
 */
 int ttc_wr_id_buffer_out(uint8_t adr, uint32_t val);

/**
 * \brief Writes a multiple parameters of the memory map to the output buffer.
 *
 * \param[in] cmd is the OBC/UHF command. Full telemetry or simplified beacon telemetry is supported.
 *
 * \return The status/error code.
 */
int ttc_wr_packet_buffer_out(uint8_t cmd);
/**
 * \brief Writes an acknowledge or error byte to the output buffer.
 *
 * \param[in] err is the error code given by another function.
 *
 * \return The status/error code of buffer writing.
 */
 int ttc_wr_ack_buffer_out(int err);

/**
 * \brief Sends data to the TTC.
 *
 * \return The status/error code.
 */
int ttc_answer_long(uint8_t *buf, uint8_t len);

/**
 * \brief Computes the CRC-8 of a sequence of bytes.
 *
 * \param[in] data is an array of data to compute the CRC-8.
 *
 * \param[in] len is the number of bytes of the given array.
 *
 * \return The computed CRC-8 value of the given data.
 */
uint8_t ttc_crc8(uint8_t *data, uint8_t len);

/**
 * \brief Checks the CRC value of a given sequence of bytes.
 *
 * \param[in] data is the data to check the CRC.
 *
 * \param[in] len is the number of bytes to check the CRC value.
 *
 * \param[in] crc is the CRC-8 value to check.
 *
 * \return TRUE/FALSE if the given CRC value is correct or not.
 */
bool ttc_check_crc(uint8_t *data, uint8_t len, uint8_t crc);

#endif /* TTC_H_ */

/** \} End of ttc group */

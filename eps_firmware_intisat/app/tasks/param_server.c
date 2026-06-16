/*
 * param_server.h
 * 
 * Copyright The EPS 2.0 Contributors.
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
 * along with EPS 2.0. If not, see <http:/\/www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief Parameter server task implementation.
 * 
 * \author Alvaro Aranibar Roque <aranibar.a@pucp.edu.pe>
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 * 
 * \version 0.2.41
 * 
 * \date 2021/07/24
 * 
 * \addtogroup param_server
 * \{
 */

#include <system/sys_log/sys_log.h>
#include <structs/eps2_data.h>

#include <drivers/i2c_slave/i2c_slave.h>
#include <drivers/uart_interrupt/uart_interrupt.h>

#include <devices/ttc/ttc.h>
#include <devices/obdh/obdh.h>
#include <devices/power_switch/power_switch.h>

#include "param_server.h"
#include "startup.h"

#include <devices/temp_sensor/temp_sensor.h>

xTaskHandle xTaskParamServerHandle;

void vTaskParamServer(void *pvParameters)
{
    BaseType_t result;
    uint32_t notified_value;

    /* Wait startup task to finish */
    xEventGroupWaitBits(task_startup_status, TASK_STARTUP_DONE, pdFALSE, pdTRUE, pdMS_TO_TICKS(TASK_PARAM_SERVER_INIT_TIMEOUT_MS));

    while(1)
    {
        result = xTaskNotifyWait(0UL, 0xFFFFFFFFUL, &notified_value, pdMS_TO_TICKS(TASK_PARAM_SERVER_MAX_BLOCK_TIME_MS));

        uint8_t adr = 0;
        uint32_t val = 0;
        uint8_t cmd = 0;

        if (result == pdPASS)
        {
            /* Process interrupt from SAFE_MODE ISR*/
            if ((notified_value & NOTIFICATION_VALUE_FROM_SAFE_MODE_ISR) != 0)
            {
                val = (uint8_t) 0x00;
                /* It is used | instead of || to ensure both operands are executed and avoid short-circuit evaluation. */
                if ( (eps_buffer_write(EPS2_PARAM_ID_POWER_EN_REG, val) | eps_buffer_write(EPS2_PARAM_ID_BUCK_EN_REG, val) ) != 0)
                {
                    sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "SAFE_MODE has failed to complete!");
                    sys_log_new_line();
                }
            }
            /* Process interrupt from I2C ISR. */
            if ((notified_value & NOTIFICATION_VALUE_FROM_I2C_ISR) != 0)
            {
                if (obc_decode(&adr, &val, &cmd) == 0)
                {
                    switch(cmd)
                    {
                        case REQ_FULL_TLM:
                        case REQ_BEACON_TLM:
                            if (obc_wr_packet_buffer_out(cmd) != 0){
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "I2C REQ_FULL_TLM or REQ_BEACON_TLM command has failed to complete (answer)!");
                                sys_log_new_line();
                            }
                            break;
                        case REQ_SINGLE_PARAM:
                            if (eps_buffer_read(adr, &val) == 0)
                            {
                                if (obc_wr_id_buffer_out(adr, val) != 0)
                                {
                                    sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "I2C REQ_SINGLE_PARAM command has failed to complete (answer)!");
                                    sys_log_new_line();
                                }
                            }
                            else
                            {
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "I2C REQ_SINGLE_PARAM command has failed to complete (buffer)!");
                                sys_log_new_line();
                            }
                            break;
                        case WR_SINGLE_PARAM:
                            if (adr == EPS2_PARAM_ID_POWER_EN_REG)
                            {
                                power_switch_update(val);
                                obc_wr_ack_buffer_out(0);
                                break; /* Writing was already done. */
                            }
                            if (eps_buffer_write(adr, val) == 0)
                                obc_wr_ack_buffer_out(0);
                            else
                            {
                                obc_wr_ack_buffer_out(-1);
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "I2C WR_SINGLE_PARAM command has failed to complete!");
                                sys_log_new_line();
                            }
                            break;
                        case ANOMALY:
                            break;
                        default: 
                            break;
                    }
                }
                else 
                {
                    sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "The received OBC command was corrupted!");
                    sys_log_new_line();
                }  
            }
            /* Process interrupt from UART ISR. */
            if ((notified_value & NOTIFICATION_VALUE_FROM_UART_ISR) != 0)
            {
                if (ttc_decode(&adr, &val, &cmd) == 0)
                {
                    switch(cmd)
                    {
                        case REQ_FULL_TLM:
                        case REQ_BEACON_TLM:
                            if (ttc_wr_packet_buffer_out(cmd) != 0){
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "UART REQ_FULL_TLM or REQ_BEACON_TLM command has failed to complete (answer)!");
                                sys_log_new_line();
                            }
                            break;
                        case REQ_SINGLE_PARAM:
                            if (eps_buffer_read(adr, &val) == 0)
                            {
                                if (ttc_wr_id_buffer_out(adr, val) != 0)
                                {
                                    sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "UART REQ_SINGLE_PARAM command has failed to complete (answer)!");
                                    sys_log_new_line();
                                }
                            }
                            else
                            {
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "UART REQ_SINGLE_PARAM command has failed to complete (buffer)!");
                                sys_log_new_line();
                            }
                            break;
                        case WR_SINGLE_PARAM:
                            if (adr == EPS2_PARAM_ID_POWER_EN_REG)
                            {
                                power_switch_update(val);
                                ttc_wr_ack_buffer_out(0);
                                break; /* Writing was already done. */
                            }
                            if (eps_buffer_write(adr, val) == 0)
                                ttc_wr_ack_buffer_out(0);
                            else
                            {
                                ttc_wr_ack_buffer_out(-1);
                                sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "UART WR_SINGLE_PARAM command has failed to complete!");
                                sys_log_new_line();
                            }
                            break;
                        case ANOMALY:
                            break;
                        default: 
                            break;
                    }
                }
                else 
                {
                    sys_log_print_event_from_module(SYS_LOG_ERROR, TASK_PARAM_SERVER_NAME, "The received TTC command was corrupted!");
                    sys_log_new_line();
                }  
            }
        }
        else
        {
            /* xTaskNotifyWait timed out. */
            sys_log_print_event_from_module(SYS_LOG_WARNING, TASK_PARAM_SERVER_NAME, "Any command request received in the last minute");
            sys_log_new_line();
        }
    }
}

void gpio_port2_pin6_isr(void)
{
    safe_mode();        /**< Turn off all power switches and regulators. */
    
    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. If calling
    xTaskNotifyFromISR() unblocks the handling task, and the priority of
    the handling task is higher than the priority of the currently running task,
    then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(xTaskParamServerHandle, NOTIFICATION_VALUE_FROM_SAFE_MODE_ISR, eSetBits, &xHigherPriorityTaskWoken);

    /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void i2c_slave_notify_from_i2c_rx_isr(void)
{
    /* xHigherPriorityTaskWoken must be initialised to pdFALSE. If calling
    xTaskNotifyFromISR() unblocks the handling task, and the priority of
    the handling task is higher than the priority of the currently running task,
    then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(xTaskParamServerHandle, NOTIFICATION_VALUE_FROM_I2C_ISR, eSetBits, &xHigherPriorityTaskWoken);

    /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void uart_interrupt_notify_from_rcv_isr(void)
{
    /* xHigherPriorityTaskWoken must be initialised to pdFALSE.  If calling
    xTaskNotifyFromISR() unblocks the handling task, and the priority of
    the handling task is higher than the priority of the currently running task,
    then xHigherPriorityTaskWoken will automatically get set to pdTRUE. */
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    xTaskNotifyFromISR(xTaskParamServerHandle, NOTIFICATION_VALUE_FROM_UART_ISR, eSetBits, &xHigherPriorityTaskWoken);

    /* Force a context switch if xHigherPriorityTaskWoken is now set to pdTRUE. */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

/** \} End of param_server group */

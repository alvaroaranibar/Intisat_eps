/*
 * uart_interrupt.c
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
 * \brief UART with interrupt driver implementation.
 * 
 * \author Alvaro Aranibar Roque <aranibar.a@pucp.edu.pe>
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 *
 * \version 0.2.28
 * 
 * \date 2021/08/01
 * 
 * \addtogroup uart_interrupt
 * \{
 */

#include <hal/usci_a_uart.h>
#include <hal/gpio.h>

#include <config/config.h>
#include <system/sys_log/sys_log.h>

#include "uart_interrupt.h"

uint8_t uart_rx_buffer[UART_RX_BUFFER_MAX_SIZE] = {0};
uint8_t uart_rx_data_size = 0;
uint8_t uart_rx_buffer_index = 0;
uint8_t uart_rx_cmd = 0;
uint8_t uart_wr_single_param_size = 0;

/* Ideally 0xFE should never be transmited because UART is an asynchronous protocol,        */
/* EPS starts communication independently and there will never present a busy condition.    */
/* However, it's initiallized only for debugging purposes and from i2c protocol legacy.     */

uint8_t uart_tx_buffer[UART_TX_BUFFER_MAX_SIZE] = {0xFE};   

uint8_t uart_tx_buffer_data_size = 1;
uint8_t uart_tx_buffer_index = 0;

int 
uart_interrupt_init(uart_interrupt_port_t port, uart_interrupt_config_t config)
{
    USCI_A_UART_initParam uart_params = {0};

    switch(config.baudrate)
    {
        case 1200:
            uart_params.clockPrescalar      = 1665; /* 1200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 1200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 1200 bps @ 31.981568 MHz */
            break;
        case 2400:
            uart_params.clockPrescalar      = 832;  /* 2400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 14;   /* 2400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 2400 bps @ 31.981568 MHz */
            break;
        case 4800:
            uart_params.clockPrescalar      = 416;  /* 4800 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 7;    /* 4800 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 4800 bps @ 31.981568 MHz */
            break;
        case 9600:
            uart_params.clockPrescalar      = 208;  /* 9600 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 3;    /* 9600 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 9600 bps @ 31.981568 MHz */
            break;
        case 19200:
            uart_params.clockPrescalar      = 104;  /* 19200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 2;    /* 19200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 19200 bps @ 31.981568 MHz */
            break;
        case 38400:
            uart_params.clockPrescalar      = 52;   /* 38400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 1;    /* 38400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 38400 bps @ 31.981568 MHz */
            break;
        case 57600:
            uart_params.clockPrescalar      = 34;   /* 57600 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 57600 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 57600 bps @ 31.981568 MHz */
            break;
        case 115200:
            uart_params.clockPrescalar      = 17;   /* 115200 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 6;    /* 115200 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 0;    /* 115200 bps @ 31.981568 MHz */
            break;
        case 230400:
            uart_params.clockPrescalar      = 8;    /* 230400 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 11;   /* 230400 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 5;    /* 230400 bps @ 31.981568 MHz */
            break;
        case 460800:
            uart_params.clockPrescalar      = 4;    /* 460800 bps @ 31.981568 MHz */
            uart_params.firstModReg         = 3;    /* 460800 bps @ 31.981568 MHz */
            uart_params.secondModReg        = 5;    /* 460800 bps @ 31.981568 MHz */
            break;
        default:
        #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
            sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization: Invalid baudrate!");
            sys_log_new_line();
        #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            return -1;      /* Invalid baudrate value */
    }

    uart_params.selectClockSource   = USCI_A_UART_CLOCKSOURCE_SMCLK;
    uart_params.parity              = config.parity;
    uart_params.msborLsbFirst       = USCI_A_UART_LSB_FIRST;
    uart_params.numberofStopBits    = config.stop_bits;
    uart_params.uartMode            = USCI_A_UART_MODE;
    uart_params.overSampling        = USCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P2, GPIO_PIN4 + GPIO_PIN5);
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P8, GPIO_PIN2 + GPIO_PIN3);
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P9, GPIO_PIN2 + GPIO_PIN3);
            break;
        default:
            #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            return -1;      /* Invalid port */
    }

    if (USCI_A_UART_init(base_address, &uart_params) != STATUS_SUCCESS)
    {
        #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
            sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during the initialization!");
            sys_log_new_line();
        #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
        return -1;
    }

    return 0;
}

int uart_interrupt_enable(uart_interrupt_port_t port)
{
    uint16_t base_address;
    switch (port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            break;
        default:
            #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Invalid port!");
                sys_log_new_line();
            #endif             /* CONFIG_DRIVERS_DEBUG_ENABLED */
            return -1; /* Invalid UART port */
    }

    USCI_A_UART_enable(base_address);
    USCI_A_UART_clearInterrupt(base_address, 
        USCI_A_UART_RECEIVE_INTERRUPT
    );
    USCI_A_UART_enableInterrupt(base_address, 
        USCI_A_UART_RECEIVE_INTERRUPT
    );

    return 0;
};

int uart_interrupt_disable(uart_interrupt_port_t port)
{
    uint16_t base_address;
    switch (port)
    {
        case UART_PORT_0:
            base_address = USCI_A0_BASE;
            break;
        case UART_PORT_1:
            base_address = USCI_A1_BASE;
            break;
        case UART_PORT_2:
            base_address = USCI_A2_BASE;
            break;
        default:
            #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Invalid port!");
                sys_log_new_line();
            #endif             /* CONFIG_DRIVERS_DEBUG_ENABLED */
            return -1; /* Invalid UART port */
    }

    USCI_A_UART_disable(base_address);
    USCI_A_UART_disableInterrupt(base_address,
        USCI_A_UART_RECEIVE_INTERRUPT +
        USCI_A_UART_TRANSMIT_INTERRUPT
    );

    return 0;
};

int uart_interrupt_write(uart_interrupt_port_t port, uint8_t *data, uint16_t len)
{
    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:   
            base_address = USCI_A0_BASE;    
            break;
        case UART_PORT_1:   
            base_address = USCI_A1_BASE;    
            break;
        case UART_PORT_2:   
            base_address = USCI_A2_BASE;    
            break;
        default:
            #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during writing: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            return -1;
    }

    memcpy(uart_tx_buffer, data, len);
    uart_tx_buffer_data_size = len;
    uart_tx_buffer_index = 0;
    
    USCI_A_UART_clearInterrupt(base_address, USCI_A_UART_TRANSMIT_INTERRUPT);
    USCI_A_UART_enableInterrupt(base_address, USCI_A_UART_TRANSMIT_INTERRUPT);

    USCI_A_UART_transmitData(base_address, uart_tx_buffer[uart_tx_buffer_index++]);

    return 0;
}

int uart_interrupt_read(uart_interrupt_port_t port, uint8_t *data, uint16_t *len, uint8_t *cmd, uint8_t* wr_single_param_size)
{
    int err = 0;
    uint16_t base_address;

    switch(port)
    {
        case UART_PORT_0:   
            base_address = USCI_A0_BASE;    
            break;
        case UART_PORT_1:   
            base_address = USCI_A1_BASE;    
            break;
        case UART_PORT_2:   
            base_address = USCI_A2_BASE;    
            break;
        default:
            #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
                sys_log_print_event_from_module(SYS_LOG_ERROR, UART_INTERRUPT_MODULE_NAME, "Error during writing: Invalid port!");
                sys_log_new_line();
            #endif /* CONFIG_DRIVERS_DEBUG_ENABLED */
            err = -1;
    }

    if (err == 0)
    {
        memcpy(data, uart_rx_buffer, uart_rx_data_size);
        *len = uart_rx_data_size;
        *cmd = uart_rx_cmd;
        *wr_single_param_size = uart_wr_single_param_size;
    }

    return err;
}

/**
 * \brief UART interrupt service routine (interrupt handler). Implemented only for USCI_A0.
 */
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt
#elif defined(__GNUC__)
__attribute__((interrupt(USCI_A0_VECTOR)))
#endif
void USCI_A0_ISR(void)
{
    switch(__even_in_range(UCA0IV,USCI_UART_UCTXCPTIFG))
    {
        case USCI_UART_UCRXIFG:
            uart_rx_buffer[uart_rx_buffer_index] = UCA0RXBUF;
            
            if (uart_rx_buffer_index++ == 0) {
                uart_rx_cmd = uart_rx_buffer[0];
            }
            else
            {
                switch(uart_rx_cmd)
                {
                    case REQ_FULL_TLM:
                        if(uart_rx_buffer_index >= FULL_TLM_RX_FR_SIZE){
                            uart_rx_data_size = uart_rx_buffer_index;
                            uart_rx_buffer_index = 0;
                            uart_interrupt_notify_from_rcv_isr();
                        }
                        break;
                    case REQ_BEACON_TLM:
                        if(uart_rx_buffer_index >= BEACON_TLM_RX_FR_SIZE){
                            uart_rx_data_size = uart_rx_buffer_index;
                            uart_rx_buffer_index = 0;
                            uart_interrupt_notify_from_rcv_isr();
                        }
                        break;
                    case REQ_SINGLE_PARAM:
                        if(uart_rx_buffer_index >= REQ_SINGLE_PARAM_RX_FR_SIZE){
                            uart_rx_data_size = uart_rx_buffer_index;
                            uart_rx_buffer_index = 0;
                            uart_interrupt_notify_from_rcv_isr();
                        }
                        break;
                    case WR_SINGLE_PARAM:
                        if( (uart_rx_buffer_index-1) == 2)       // Decrement 1 to compensate previous increment.
                        {
                            uart_wr_single_param_size = uart_rx_buffer[uart_rx_buffer_index-1];
                        }
                        else if ( (uart_rx_buffer_index) == (uart_wr_single_param_size + 4)) 
                        {
                            uart_rx_data_size = uart_rx_buffer_index;
                            uart_rx_buffer_index = 0;
                            uart_interrupt_notify_from_rcv_isr();
                        }
                        break;
                    default:
                        break;
                }
                
                /* Avoid buffer overflow if wr payload size is incorrect. */
                if (uart_rx_buffer_index >= UART_RX_BUFFER_MAX_SIZE)
                {
                    uart_rx_buffer_index = 0; /* Delete trama. */
                }
            }    
            break;

        case USCI_UART_UCTXIFG:
            if (uart_tx_buffer_index < uart_tx_buffer_data_size) 
            {
                USCI_A_UART_transmitData(USCI_A0_BASE, uart_tx_buffer[uart_tx_buffer_index++]);
            }
            else 
            {
                USCI_A_UART_disableInterrupt(USCI_A0_BASE, USCI_A_UART_TRANSMIT_INTERRUPT);
                uart_tx_buffer_index = 0;
                uart_tx_buffer[0] = 0XFE;    
                uart_tx_buffer_data_size = 1;
            }
            break;
        default: 
            break;  
    }
}

__attribute__((weak)) void uart_interrupt_notify_from_rcv_isr(void)
{
    #if CONFIG_DRIVERS_DEBUG_ENABLED == 1
        sys_log_print_event_from_module(SYS_LOG_INFO, UART_INTERRUPT_MODULE_NAME, "Notified to uart rcv handler");
        sys_log_new_line();
    #endif
}

/** \} End of uart group */

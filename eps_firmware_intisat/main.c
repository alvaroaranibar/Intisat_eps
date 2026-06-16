/*
 * main.c
 * 
 * 
 * This file is part of EPS INTISAT
 * 
 * EPS INTISAT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * EPS INTISAT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with EPS INTISAT. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief Main file.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * \author Alvaro Aranibar Roque <aranibar.a@pucp.edu.pe>
 * 
 * \version 0.2.0
 * 
 * \date 2026/03/13
 * 
 * \defgroup main Main file
 * \{
 */

#include <FreeRTOS.h>
#include <task.h>

#include "devices/watchdog/watchdog.h"
#include "system/clocks.h"
#include "app/tasks/tasks.h"

void main(void)
{
    /* Disable default watchdog immediately to prevent reset during clock setup */
    WDT_A_hold(WDT_A_BASE);

    /* System clocks configuration */
    clocks_setup((clocks_config_t){.mclk_hz = 32000000UL, .smclk_hz=32000000UL, .aclk_hz=32768});

    /* Watchdog initialization: ACLK/32K = 1 s timeout, fed every 100 ms by vTaskWatchdogReset */
    watchdog_init();

    /* Create all the tasks */
    create_tasks();

    /* Start the scheduler */
    vTaskStartScheduler();

    /* Will only get here if there was insufficient memory to create the idle and/or timer task */
    while(1);
}

/** \} End of main group */

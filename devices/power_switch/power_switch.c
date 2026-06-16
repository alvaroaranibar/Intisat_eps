/* Falta cabezera */

#include <system/sys_log/sys_log.h>
#include <drivers/gpio/gpio.h>
#include <app/structs/eps2_data.h>
#include <hal/gpio.h>

#include "power_switch.h"

/* GPIO configuration */

#define EN_3V3_2A_PIN       GPIO_PIN_54
#define EN_3V3_PSW0_PIN     GPIO_PIN_38
#define EN_3V3_PSW1_PIN     GPIO_PIN_34
#define EN_5V_3A_PIN        GPIO_PIN_3
#define EN_5V_5A_1_PIN      GPIO_PIN_62
#define EN_5V_PSW0_PIN      GPIO_PIN_37
#define EN_5V_PSW1_PIN      GPIO_PIN_0
#define EN_5V_5A_0_PIN      GPIO_PIN_39
#define EN_5V_PSW2_PIN      GPIO_PIN_1
#define EN_5V_PSW3_PIN      GPIO_PIN_2

int power_switch_init()
{
    sys_log_print_event_from_module(SYS_LOG_INFO, POWER_SWITCH_MODULE_NAME, "Initializing power switches and regulators enable.");
    sys_log_new_line();

    gpio_config_t config_3v3_2a   = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_3v3_psw0 = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_3v3_psw1 = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_3a    = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_5a_1  = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_psw0  = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_psw1  = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_5a_0  = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_psw2  = {.mode = GPIO_MODE_OUTPUT};
    gpio_config_t config_5v_psw3  = {.mode = GPIO_MODE_OUTPUT};

    if (
        gpio_init(EN_3V3_2A_PIN, config_3v3_2a)     ||
        gpio_init(EN_3V3_PSW0_PIN, config_3v3_psw0) ||
        gpio_init(EN_3V3_PSW1_PIN, config_3v3_psw1) ||
        gpio_init(EN_5V_3A_PIN, config_5v_3a)           ||
        gpio_init(EN_5V_5A_1_PIN, config_5v_5a_1)   ||
        gpio_init(EN_5V_PSW0_PIN, config_5v_psw0)   ||
        gpio_init(EN_5V_PSW1_PIN, config_5v_psw1)   ||
        gpio_init(EN_5V_5A_0_PIN, config_5v_5a_0)   ||
        gpio_init(EN_5V_PSW2_PIN, config_5v_psw2)   ||
        gpio_init(EN_5V_PSW3_PIN, config_5v_psw3)      
       )
    {
        sys_log_print_event_from_module(SYS_LOG_ERROR, POWER_SWITCH_MODULE_NAME, "Error initializing regulators and power switches enable!");
        sys_log_new_line();

        return -1;
    }

    /* All controllable power switches and regulators are initially disabled. */
    if (
        gpio_set_state(EN_3V3_2A_PIN, false)     ||
        gpio_set_state(EN_3V3_PSW0_PIN, false) ||
        gpio_set_state(EN_3V3_PSW1_PIN,false ) ||
        gpio_set_state(EN_5V_3A_PIN, true)           ||
        gpio_set_state(EN_5V_5A_1_PIN, false)   ||
        gpio_set_state(EN_5V_PSW0_PIN, false)   ||
        gpio_set_state(EN_5V_PSW1_PIN, false)   ||
        gpio_set_state(EN_5V_5A_0_PIN, false)   ||
        gpio_set_state(EN_5V_PSW2_PIN, false)   ||
        gpio_set_state(EN_5V_PSW3_PIN, false)      
       )
    {
        sys_log_print_event_from_module(SYS_LOG_ERROR, POWER_SWITCH_MODULE_NAME, "Error while initially disabling regulators and power switches! ");
        sys_log_new_line();

        return -1;
    }
    return 0;
}

int power_switch_update(uint8_t power_en_reg) 
{
    uint8_t buck_en_reg = 0;

    /* Pre-calculate the state of the buck regulators using bitmasks.
       Any non-zero value in the mask evaluates to 'true' (1). */
    uint8_t buck_3v3_2a  = (power_en_reg & 0x60) ? 1 : 0; // Evaluates bits 6 and 5
    uint8_t buck_5v_5a_1 = (power_en_reg & 0x0C) ? 1 : 0; // Evaluates bits 3 and 2
    uint8_t buck_5v_5a_0 = (power_en_reg & 0x03) ? 1 : 0; // Evaluates bits 1 and 0

    /* Mask bit 7 (reserved) to force it always to 0. */
    power_en_reg &= 0x7F;

    /* Update GPIOs for the 3.3V stage */
    gpio_set_state(EN_3V3_2A_PIN,   buck_3v3_2a);
    gpio_set_state(EN_3V3_PSW0_PIN, (power_en_reg & (1 << 6)) ? 1 : 0); 
    gpio_set_state(EN_3V3_PSW1_PIN, (power_en_reg & (1 << 5)) ? 1 : 0); 

    /* Update GPIO for the 5V_3A stage (Independent buck) */
    gpio_set_state(EN_5V_3A_PIN,    (power_en_reg & (1 << 4)) ? 1 : 0); 

    /* Update GPIOs for the 5V_5A_1 stage */
    gpio_set_state(EN_5V_5A_1_PIN,  buck_5v_5a_1);
    gpio_set_state(EN_5V_PSW0_PIN,  (power_en_reg & (1 << 3)) ? 1 : 0); 
    gpio_set_state(EN_5V_PSW1_PIN,  (power_en_reg & (1 << 2)) ? 1 : 0); 

    /* Update GPIOs for the 5V_5A_0 stage */
    gpio_set_state(EN_5V_5A_0_PIN,  buck_5v_5a_0);
    gpio_set_state(EN_5V_PSW2_PIN,  (power_en_reg & (1 << 1)) ? 1 : 0);
    gpio_set_state(EN_5V_PSW3_PIN,  (power_en_reg & (1 << 0)) ? 1 : 0);

    /* Assemble the buck status register using pre-calculated variables */
    buck_en_reg = (buck_3v3_2a << 2) | (buck_5v_5a_1 << 1) | (buck_5v_5a_0);

    /* Write to the EPS buffer */
    eps_buffer_write(EPS2_PARAM_ID_POWER_EN_REG, (uint32_t)power_en_reg);
    eps_buffer_write(EPS2_PARAM_ID_BUCK_EN_REG, (uint32_t)buck_en_reg);
    
    /* As there lots of set functions, it's assumed that GPIO pin given to function is OK (No validation)*/
    return 0;
}

void safe_mode(void) {
    /* Neither driverlib, nor gpio_set_state are used as maximum execution speed is required inside the ISR. */
    
    P8OUT &= ~BIT0;                         /**< Turn off EN_3V3_2A_PIN. */
    P5OUT &= ~(BIT6 | BIT2 | BIT5 | BIT7);  /**< Turn off EN_3V3_PSW0, EN_3V3_PSW1, EN_5V_PSW0, EN_5V_5A_0. */
    P1OUT &= ~(BIT3 | BIT0 | BIT1 | BIT2);  /**< Turn off EN_5V_3A, EN_5V_PSW1, EN_5V_PSW2, EN_5V_PSW3. */
    P9OUT &= ~BIT0;                         /**< Turn off EN_5V_5A_1_PIN. */

    /* Driver lib version of the function */
    //GPIO_setOutputLowOnPin(GPIO_PORT_P8,   GPIO_PIN0);  /**< Turn off EN_3V3_2A_PIN. */
    //GPIO_setOutputLowOnPin(GPIO_PORT_P5,   GPIO_PIN6    /**< Turn off EN_3V3_PSW0_PIN. */
    //                                     | GPIO_PIN2    /**< Turn off EN_3V3_PSW1_PIN. */
    //                                     | GPIO_PIN5    /**< Turn off EN_5V_PSW0_PIN. */
    //                                     | GPIO_PIN7);  /**< Turn off EN_5V_5A_0_PIN. */
    //GPIO_setOutputLowOnPin(GPIO_PORT_P1,   GPIO_PIN3    /**< Turn off EN_5V_3A_PIN. */ 
    //                                     | GPIO_PIN0    /**< Turn off EN_5V_PSW1_PIN. */ 
    //                                     | GPIO_PIN1    /**< Turn off EN_5V_PSW2_PIN. */ 
    //                                     | GPIO_PIN2);  /**< Turn off EN_5V_PSW3_PIN. */
    //GPIO_setOutputLowOnPin(GPIO_PORT_P9,   GPIO_PIN0);  /**< Turn off EN_5V_5A_1_PIN. */  
}

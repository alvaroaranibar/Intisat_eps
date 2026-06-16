#include <system/sys_log/sys_log.h>
#include <drivers/gpio/gpio.h>
#include <hal/gpio.h>
#include <devices/gpio_alerts/gpio_alerts.h>

#define EPS_ALERT   GPIO_PIN_15     /**< PC104_GPIO0 */
#define SAFE_MODE   GPIO_PIN_14     /**< PC104_GPIO1 */

int gpio_alerts_init(void)
{
    gpio_config_t config_eps_alert = {.mode = GPIO_MODE_OUTPUT};
    if ( gpio_init(EPS_ALERT, config_eps_alert))
    {
        sys_log_print_event_from_module(SYS_LOG_ERROR, GPIO_ALERTS_MODULE_NAME, "Error initializing EPS_ALERT pin!");
        sys_log_new_line();

        return -1;
    }

    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN6); /**< SAFE_MODE pin with internal pull-up resistor */
    GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN6);                 /**< Enable interrupt for SAFE_MODE pin. */
    GPIO_selectInterruptEdge(   GPIO_PORT_P2, 
                                GPIO_PIN6, 
                                GPIO_HIGH_TO_LOW_TRANSITION); /**< SAFE_MODE Active-low enable as it has pull-up resistor */
    GPIO_clearInterrupt(GPIO_PORT_P2, GPIO_PIN6);

    return 0;
}

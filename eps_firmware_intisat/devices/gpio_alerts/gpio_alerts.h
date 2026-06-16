#ifndef GPIO_ALERTS_H_
#define GPIO_ALERTS_H

#include <stdint.h>

#define GPIO_ALERTS_MODULE_NAME "GPIO Alerts"

/**
 * \brief Initialization routine of GPIO Alerts pins and interrupts.
 *
 * \return The status/error code.
 */
int gpio_alerts_init(void);

#endif /* GPIO_ALERTS_H_ */

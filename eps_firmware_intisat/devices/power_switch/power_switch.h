/* Falta incluir cabezera */

#ifndef POWER_SWITCH_H_
#define POWER_SWITCH_H_

#include <stdint.h>

#define POWER_SWITCH_MODULE_NAME "Power Switches"

/**
 * \brief Initialization routine of regulators and power switches.
 *
 * \return The status/error code.
 */
int power_switch_init();

/**
 * \brief Turns on/off power switches and regulators according to the power switches enable register.
 *
 * \param[in] power_en_reg Power switches enable register
 *
 * \return The status/error code.
 */
int power_switch_update(uint8_t power_en_reg) ;

/**
 * \brief Turns off all regulator and power switches.
 */
void safe_mode(void);

#endif /* POWER_SWITCH_H */

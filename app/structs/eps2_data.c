/*
 * eps2_data.c
 *
 * Copyright (C) 2021, SpaceLab.
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
 * \brief EPS data structure implementation.
 *
 * \author Yan Castro de Azeredo <yan.ufsceel@gmail.com>
 * \author André M. P. de Mattos <andre.mattos@spacelab.ufsc.br>
 *
 * \version 0.2.17
 *
 * \date 2021/03/31
 *
 * \addgroup epd_data
 * \{
 */

#include <system/sys_log/sys_log.h>

#include "eps2_data.h"

uint8_t eps_param_id_beacon[EPS_BEACON_NUMBER_PARAM] = {EPS2_PARAM_ID_BAT_VOLTAGE,
                                                        EPS2_PARAM_ID_RTD_0_TEMP,
                                                        EPS2_PARAM_ID_RTD_1_TEMP,
                                                        EPS2_PARAM_ID_RTD_4_TEMP,
                                                        EPS2_PARAM_ID_RTD_5_TEMP,
                                                        EPS2_PARAM_ID_BAT_MONITOR_RARC,
                                                        EPS2_PARAM_ID_SP_MY_PX_VOLTAGE,
                                                        EPS2_PARAM_ID_SP_MX_PZ_VOLTAGE,
                                                        EPS2_PARAM_ID_SP_MZ_PY_VOLTAGE,
                                                        EPS2_PARAM_ID_SP_MY_CURRENT,
                                                        EPS2_PARAM_ID_SP_PY_CURRENT,
                                                        EPS2_PARAM_ID_SP_MX_CURRENT,
                                                        EPS2_PARAM_ID_SP_PX_CURRENT,
                                                        EPS2_PARAM_ID_SP_MZ_CURRENT,
                                                        EPS2_PARAM_ID_SP_PZ_CURRENT
                                                       };      

eps_data_t eps_data_buff = {
    // EPS data buffer initial/default values. All others are initialized with 0.
    .mppt_1_mode = 0,
    .mppt_1_duty_cycle = 50,

    .mppt_2_mode = 0,
    .mppt_2_duty_cycle = 50,
    
    .mppt_3_mode = 0,
    .mppt_3_duty_cycle = 50,
    
    .heater1_mode = 1,
    .heater1_duty_cycle = 0,
    
    .heater2_mode = 1,
    .heater2_duty_cycle = 0,
    
    .firmware_version = 0x00000300,       

    .device_id = 0xEEE2
};

int eps_buffer_write(uint8_t id, uint32_t value)
{
	switch(id)
    {
        case EPS2_PARAM_ID_TIME_COUNTER:
        	eps_data_buff.time_counter_ms = value;
            break;
        case EPS2_PARAM_ID_MCU_TEMP:
            eps_data_buff.eps_mcu_temp_kelvin = value;
            break;
        case EPS2_PARAM_ID_EPS_CURRENT:
            eps_data_buff.eps_beacon_ma = value;
            break;
        case EPS2_PARAM_ID_LAST_RESET_CAUSE:
            eps_data_buff.last_reset_cause = value;
            break;
        case EPS2_PARAM_ID_RESET_COUNTER:
            eps_data_buff.reset_counter = value;
            break;
        case EPS2_PARAM_ID_SP_MY_PX_VOLTAGE:
            eps_data_buff.minus_y_plus_x_solar_panel_mv = value;
            break;
        case EPS2_PARAM_ID_SP_MX_PZ_VOLTAGE:
            eps_data_buff.minus_x_plus_z_solar_panel_mv = value;
            break;
        case EPS2_PARAM_ID_SP_MZ_PY_VOLTAGE:
            eps_data_buff.minus_z_plus_y_solar_panel_mv = value;
            break;
        case EPS2_PARAM_ID_SP_MY_CURRENT:
            eps_data_buff.minus_y_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_SP_PY_CURRENT:
            eps_data_buff.plus_y_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_SP_MX_CURRENT:
            eps_data_buff.minus_x_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_SP_PX_CURRENT:
            eps_data_buff.plus_x_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_SP_MZ_CURRENT:
            eps_data_buff.minus_z_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_SP_PZ_CURRENT:
            eps_data_buff.plus_z_solar_panel_ma = value;
            break;
        case EPS2_PARAM_ID_MPPT_1_DUTY_CYCLE:
            eps_data_buff.mppt_1_duty_cycle = value;
            break;
        case EPS2_PARAM_ID_MPPT_2_DUTY_CYCLE:
            eps_data_buff.mppt_2_duty_cycle = value;
            break;
        case EPS2_PARAM_ID_MPPT_3_DUTY_CYCLE:
            eps_data_buff.mppt_3_duty_cycle = value;
            break;
        case EPS2_PARAM_ID_SP_VOLTAGE_MPPT:
            eps_data_buff.total_solar_panels_mv = value;
            break;
        case EPS2_PARAM_ID_MAIN_POWER_BUS_VOLTAGE:
            eps_data_buff.main_power_buss_mv = value;
            break;
        case EPS2_PARAM_ID_RTD_0_TEMP:
            eps_data_buff.batteries_rtd0_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_1_TEMP:
            eps_data_buff.batteries_rtd1_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_2_TEMP:
            eps_data_buff.batteries_rtd2_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_3_TEMP:
            eps_data_buff.batteries_rtd3_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_4_TEMP:
            eps_data_buff.solar_panels_rtd4_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_5_TEMP:
            eps_data_buff.solar_panels_rtd5_kelvin = value;
            break;
        case EPS2_PARAM_ID_RTD_6_TEMP:
            eps_data_buff.solar_panels_rtd6_kelvin = value;
            break;
        case EPS2_PARAM_ID_BAT_VOLTAGE:
            eps_data_buff.batteries_mv = value;
            break;
        case EPS2_PARAM_ID_BAT_CURRENT:
            eps_data_buff.batteries_ma = value;
            break;
        case EPS2_PARAM_ID_BAT_AVERAGE_CURRENT:
            eps_data_buff.batteries_average_ma = value;
            break;
        case EPS2_PARAM_ID_BAT_ACC_CURRENT:
            eps_data_buff.batteries_accumulated_ma = value;
            break;
        case EPS2_PARAM_ID_BAT_CHARGE:
            eps_data_buff.batteries_charge_mah = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_TEMP:
            eps_data_buff.bm_temp_kelvin = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_STATUS:
            eps_data_buff.bm_status_reg = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_PROTECT:
            eps_data_buff.bm_protection_reg = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_CYCLE_COUNTER:
            eps_data_buff.bm_cycle_counter_reg = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RAAC:
            eps_data_buff.bm_raac_mah = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RSAC:
            eps_data_buff.bm_rsac_mah = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RARC:
            eps_data_buff.bm_rarc_percent = value;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RSRC:
            eps_data_buff.bm_rsrc_percent = value;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_1_DUTY_CYCLE:
            eps_data_buff.heater1_duty_cycle = value;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_2_DUTY_CYCLE:
            eps_data_buff.heater2_duty_cycle = value;
            break;
        case EPS2_PARAM_ID_FW_VERSION:
            eps_data_buff.firmware_version = value;
            break;
        case EPS2_PARAM_ID_MPPT_1_MODE:
            eps_data_buff.mppt_1_mode = value;
            break;
        case EPS2_PARAM_ID_MPPT_2_MODE:
            eps_data_buff.mppt_2_mode = value;
            break;
        case EPS2_PARAM_ID_MPPT_3_MODE:
            eps_data_buff.mppt_3_mode = value;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_1_MODE:
            eps_data_buff.heater1_mode = value;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_2_MODE:
            eps_data_buff.heater2_mode = value;
            break;
        case EPS2_PARAM_ID_POWER_EN_REG:
            eps_data_buff.power_en_reg = value;
            break;
        case EPS2_PARAM_ID_BUCK_EN_REG:
            eps_data_buff.buck_en_reg = value;
            break;
        case EPS2_PARAM_ID_DEVICE_ID:
            eps_data_buff.device_id = value;
            break;
        default:
            sys_log_print_event_from_module(SYS_LOG_ERROR, EPS_DATA_NAME, "Unknown parameter ID while writing!");
            sys_log_new_line();
            return -1;
    }

    return 0;
}

int eps_buffer_read(uint8_t id, uint32_t *value)
{

    #if CONFIG_SET_DUMMY_EPS == 1
    switch(id)
    {
        /* ================================================================
         * NON-CRITICAL PARAMETERS — zeroed out for focused OBC testing
         * ================================================================ */
        case EPS2_PARAM_ID_TIME_COUNTER:              *value = 0; break;
        case EPS2_PARAM_ID_MCU_TEMP:                  *value = 0; break;
        case EPS2_PARAM_ID_EPS_CURRENT:               *value = 0; break;
        case EPS2_PARAM_ID_LAST_RESET_CAUSE:          *value = 0; break;
        case EPS2_PARAM_ID_SP_MY_PX_VOLTAGE:          *value = 0; break;
        case EPS2_PARAM_ID_SP_MX_PZ_VOLTAGE:          *value = 0; break;
        case EPS2_PARAM_ID_SP_MZ_PY_VOLTAGE:          *value = 0; break;
        case EPS2_PARAM_ID_SP_MY_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_SP_PY_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_SP_MX_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_SP_PX_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_SP_MZ_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_SP_PZ_CURRENT:             *value = 0; break;
        case EPS2_PARAM_ID_MPPT_1_DUTY_CYCLE:         *value = 0; break;
        case EPS2_PARAM_ID_MPPT_2_DUTY_CYCLE:         *value = 0; break;
        case EPS2_PARAM_ID_MPPT_3_DUTY_CYCLE:         *value = 0; break;
        case EPS2_PARAM_ID_SP_VOLTAGE_MPPT:           *value = 0; break;
        case EPS2_PARAM_ID_RTD_1_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_RTD_2_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_RTD_3_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_RTD_4_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_RTD_5_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_RTD_6_TEMP:                *value = 0; break;
        case EPS2_PARAM_ID_BAT_CURRENT:               *value = 0; break;
        case EPS2_PARAM_ID_BAT_AVERAGE_CURRENT:       *value = 0; break;
        case EPS2_PARAM_ID_BAT_ACC_CURRENT:           *value = 0; break;
        case EPS2_PARAM_ID_BAT_CHARGE:                *value = 0; break;
        case EPS2_PARAM_ID_BAT_MONITOR_TEMP:          *value = 0; break;
        case EPS2_PARAM_ID_BAT_MONITOR_CYCLE_COUNTER: *value = 0; break;
        case EPS2_PARAM_ID_BAT_MONITOR_RAAC:          *value = 0; break;
        case EPS2_PARAM_ID_BAT_MONITOR_RSAC:          *value = 0; break;
        case EPS2_PARAM_ID_BAT_HEATER_1_DUTY_CYCLE:  *value = 0; break;
        case EPS2_PARAM_ID_BAT_HEATER_2_DUTY_CYCLE:  *value = 0; break;
        case EPS2_PARAM_ID_MPPT_1_MODE:               *value = 0; break;
        case EPS2_PARAM_ID_MPPT_2_MODE:               *value = 0; break;
        case EPS2_PARAM_ID_MPPT_3_MODE:               *value = 0; break;
        case EPS2_PARAM_ID_BAT_HEATER_1_MODE:         *value = 0; break;
        case EPS2_PARAM_ID_BAT_HEATER_2_MODE:         *value = 0; break;
        case EPS2_PARAM_ID_FW_VERSION:                *value = 0; break;
        case EPS2_PARAM_ID_DEVICE_ID:                 *value = 0xEEE2U; break;

        /* ================================================================
         * CRITICAL HEALTH PARAMETERS — healthy dummy values for OBC testing
         * Battery pack: 2S2P Li-ion
         *   Nominal: 7200 mV | Max: 8400 mV | Capacity: 2450 mAh
         *   Active cutoff (3.2V/cell): 6400 mV
         *   Standby cutoff (2.8V/cell): 5600 mV
         *   Active discharge rate: 6 A | Standby rate: 1 A
         * OBC polling: REQ_FULL_TLM every 3 min, full structure saved to flash.
         * NOTE: 3-min snapshot IS the log. WARNING states require no OBC write.
         *
         * SAFE MODE definition: write power_en_reg = 0x00 via WR_SINGLE_PARAM
         *   (all controllable switches off). UHF, OBC and EPS remain powered
         *   by hardware regardless. Specific switch sequencing TBD based on
         *   satellite operating states (to be defined once PSW assignments known).
         * RECOVERY from SAFE MODE: restore previous power_en_reg value and
         *   switch OBC state machine back to NORMAL only when BOTH RARC > 50%
         *   AND RSRC > 50%.
         * ================================================================ */

        /* --- Battery raw voltage [mV] ---
         * BACKUP CHECK for RARC/RSRC: RARC/RSRC are the primary safe mode
         * triggers. This register is a last-resort confirmation if SoC failed.
         *
         * > 6400 mV  [GOOD]: No action. RARC/RSRC are decision makers here.
         *
         * < 6400 mV  [SAFE MODE — BACKUP]: Active cutoff violated.
         *            If NOT already in SAFE_MODE: enter SAFE MODE now.
         *            If already in SAFE_MODE: expected state, keep monitoring.
         *
         * < 5600 mV  [EMERGENCY]: Standby cutoff violated. Both primary and
         *            backup checks failed. DS2777 hardware protection takes over. */
        case EPS2_PARAM_ID_BAT_VOLTAGE:
            *value = 7600;
            break;

        /* --- Main power bus voltage [mV] ---
         * Tracks the unregulated battery bus. Expected to match batteries_mv.
         * SECONDARY BACKUP to batteries_mv — same thresholds and actions apply.
         *
         * SENSOR INCONSISTENCY: If |main_power_buss_mv - batteries_mv| > 200 mV,
         *   use batteries_mv as primary. Inconsistency visible in next snapshot. */
        case EPS2_PARAM_ID_MAIN_POWER_BUS_VOLTAGE:
            *value = 7600;
            break;

        /* --- Active SoC at 6A discharge rate [%] ---
         * PRIMARY metric for safe mode entry while in ACTIVE MODE.
         * OBC must keep previous RARC value in RAM to detect transitions.
         *
         * > 60 %  [GOOD]: No action. 3-min snapshot records the value.
         * 40-60 % [WARNING]: No action. Degradation visible in snapshots.
         * < 30 %  [SAFE MODE]: Enter SAFE MODE (power_en_reg = 0x00).
         *          Increase poll rate: REQ_SINGLE_PARAM(RARC+RSRC) every 60 s.
         * > 50 %  [RECOVERY]: Exit SAFE MODE only if RSRC is also > 50 %. */
        case EPS2_PARAM_ID_BAT_MONITOR_RARC:
            *value = 80;
            break;

        /* --- Standby SoC at 1A discharge rate [%] ---
         * KEY metric for eclipse survivability. Always evaluated in parallel,
         * regardless of current operating mode. OBC keeps previous value in RAM.
         *
         * > 50 %  [GOOD]: No action. 3-min snapshot records the value.
         * 35-50 % [WARNING]: No action. Degradation visible in snapshots.
         * < 35 %  [SAFE MODE — FORCED]: Enter SAFE MODE (power_en_reg = 0x00)
         *          regardless of RARC. Eclipse survival at 1A not guaranteed.
         *          Increase poll rate: REQ_SINGLE_PARAM(RARC+RSRC) every 60 s.
         * > 50 %  [RECOVERY]: Exit SAFE MODE only if RARC is also > 50 %. */
        case EPS2_PARAM_ID_BAT_MONITOR_RSRC:
            *value = 85;
            break;

        /* --- DS2777 status register ---
         * 0x00   [HEALTHY]: No action. 3-min snapshot records the value.
         *
         * Any bit set, clears on next poll [TRANSIENT]: No action.
         *         Visible in snapshot sequence for ground review.
         *
         * Any bit set, persists across 3 consecutive polls (~9 min) [FAULT]:
         *         Enter SAFE MODE (power_en_reg = 0x00).
         *         Exit SAFE MODE only when register reads 0x00 for 3 polls
         *         AND both RARC > 50% and RSRC > 50%. */
        case EPS2_PARAM_ID_BAT_MONITOR_STATUS:
            *value = 0x00;
            break;

        /* --- DS2777 protection register ---
         * BACKUP CHECK for batteries_mv and RARC: hardware-enforced by the
         * battery monitor IC. Act immediately — do not wait for next 3-min poll.
         *
         * 0x00        [HEALTHY]: No action.
         *
         * OV bit set  (overvoltage): Write all MPPT duty cycles = 0 via
         *             WR_SINGLE_PARAM to stop solar charging. Resume when cleared.
         *
         * UV bit set  (undervoltage): Enter SAFE MODE (power_en_reg = 0x00).
         *
         * OC bit set  (overcurrent, physical limit 10 A): Enter SAFE MODE
         *             (power_en_reg = 0x00) immediately to reduce current draw.
         *
         * Exit SAFE MODE when register reads 0x00 for 3 consecutive polls
         * AND both RARC > 50% and RSRC > 50%. */
        case EPS2_PARAM_ID_BAT_MONITOR_PROTECT:
            *value = 0x00;
            break;

        /* --- Battery temperature RTD 0 [Kelvin] ---
         * Controls heater decisions. Default: heaters in AUTO (EPS manages them).
         * OBC only intervenes when EPS AUTO is insufficient or sensor is faulty.
         * These actions are independent of SAFE MODE — temperature management
         * continues even while in SAFE MODE.
         *
         * 273-318 K  [GOOD | 0-45 C]: No action. EPS heaters stay in AUTO.
         *
         * 263-273 K  [WARNING LOW | -10 to 0 C]: No action if heaters are AUTO.
         *             If heaters are in MANUAL (prior fault): write duty = 50 %.
         *
         * < 263 K    [ACTION LOW | < -10 C]: Switch heaters to MANUAL via
         *             WR_SINGLE_PARAM(BAT_HEATER_1_MODE, 0) and duty = 100 %.
         *             Write all MPPT duty cycles = 0 to suspend charging.
         *             Reason: charging Li-ion below 0 C causes lithium plating
         *             (irreversible cell damage).
         *             Restore AUTO and MPPT when RTD_0 returns to > 278 K (5 C).
         *
         * 318-333 K  [WARNING HIGH | 45-60 C]: Write MPPT duty cycles -= 20 %
         *             via WR_SINGLE_PARAM to reduce charging current and heat.
         *
         * > 333 K    [ACTION HIGH | > 60 C]: Write all MPPT duty cycles = 0.
         *             Restore MPPT when RTD_0 returns to < 323 K (50 C).
         *
         * < 100 K or > 400 K [IMPLAUSIBLE — sensor failure]: Switch heaters to
         *             MANUAL with duty = 20 % (conservative safe value).
         *             Flag RTD_0 as FAILED in OBC state machine.
         *             Do not use for decisions until cleared by ground command. */
        case EPS2_PARAM_ID_RTD_0_TEMP:
            *value = 293;
            break;

        /* --- Power enable register ---
         * Written by OBC to control power switches (active-high enable).
         * Bit 7: reserved (always 0)
         * Bit 6: EN_3V3_PSW0  | Bit 5: EN_3V3_PSW1
         * Bit 4: EN_5V_3A (independent buck, no cascade switch)
         * Bit 3: EN_5V_PSW0   | Bit 2: EN_5V_PSW1
         * Bit 1: EN_5V_PSW2   | Bit 0: EN_5V_PSW3
         * SAFE MODE value: 0x00 (all switches off)
         * Initial hardware state: 0x10 (EN_5V_3A on by default in EPS init)
         *
         * MATCH (read == last commanded value): No action.
         *
         * MISMATCH (read != last commanded value):
         *   Wait 1 poll (3 min) and re-read via REQ_SINGLE_PARAM.
         *   If still mismatched: check reset_counter.
         *     reset_counter incremented -> EPS reset; re-send power_en_reg.
         *     reset_counter unchanged   -> re-send power_en_reg; if persists,
         *                                  visible in next snapshot for ground. */
        case EPS2_PARAM_ID_POWER_EN_REG:
            *value = 0x10;
            break;

        /* --- Buck enable register (read-only for OBC, auto-set by EPS) ---
         * Do not write this register. EPS calculates it from power_en_reg.
         * Bit 2: buck 3V3/2A  -> expected 1 if (power_en_reg & 0x60) != 0
         * Bit 1: buck 5V/5A_1 -> expected 1 if (power_en_reg & 0x0C) != 0
         * Bit 0: buck 5V/5A_0 -> expected 1 if (power_en_reg & 0x03) != 0
         * NOTE: EN_5V_3A (bit4 of power_en_reg) is independent; not tracked here.
         *
         * CONSISTENT: No action.
         *
         * INCONSISTENT: Re-read both registers. If still inconsistent, re-write
         *   power_en_reg. If persists: clear the PSW bits driving the faulty
         *   buck stage in power_en_reg. Visible in next snapshot for ground. */
        case EPS2_PARAM_ID_BUCK_EN_REG:
            *value = 0x00;
            break;

        /* --- EPS reset counter ---
         * OBC must store the previous reset_counter value in RAM between polls.
         *
         * STABLE (no change): No action.
         *
         * INCREMENTED BY 1:
         *   Within expected auto-reset window (EPS resets every 10 h):
         *     Re-send current power_en_reg via WR_SINGLE_PARAM.
         *   Outside expected window (unexpected reset):
         *     Re-send power_en_reg. Visible in next snapshot for ground.
         *
         * INCREMENTED BY > 1 between polls (multiple resets within 3 min):
         *   EPS critically unstable. Enter SAFE MODE (power_en_reg = 0x00).
         *   Exit only when counter is stable for > 30 min and energy is healthy. */
        case EPS2_PARAM_ID_RESET_COUNTER:
            *value = 1;
            break;

        default:
            sys_log_print_event_from_module(SYS_LOG_ERROR, EPS_DATA_NAME, "Unknown parameter ID while reading!");
            sys_log_new_line();
            return -1;
    }

    return 0;
    #endif

	switch(id)
    {
        case EPS2_PARAM_ID_TIME_COUNTER:
        	*value = eps_data_buff.time_counter_ms;
            break;
        case EPS2_PARAM_ID_MCU_TEMP:
            *value = eps_data_buff.eps_mcu_temp_kelvin;
            break;
        case EPS2_PARAM_ID_EPS_CURRENT:
            *value = eps_data_buff.eps_beacon_ma;
            break;
        case EPS2_PARAM_ID_LAST_RESET_CAUSE:
            *value = eps_data_buff.last_reset_cause;
            break;
        case EPS2_PARAM_ID_RESET_COUNTER:
            *value = eps_data_buff.reset_counter;
            break;
        case EPS2_PARAM_ID_SP_MY_PX_VOLTAGE:
            *value = eps_data_buff.minus_y_plus_x_solar_panel_mv;
            break;
        case EPS2_PARAM_ID_SP_MX_PZ_VOLTAGE:
            *value = eps_data_buff.minus_x_plus_z_solar_panel_mv;
            break;
        case EPS2_PARAM_ID_SP_MZ_PY_VOLTAGE:
            *value = eps_data_buff.minus_z_plus_y_solar_panel_mv;
            break;
        case EPS2_PARAM_ID_SP_MY_CURRENT:
            *value = eps_data_buff.minus_y_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_SP_PY_CURRENT:
            *value = eps_data_buff.plus_y_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_SP_MX_CURRENT:
            *value = eps_data_buff.minus_x_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_SP_PX_CURRENT:
            *value = eps_data_buff.plus_x_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_SP_MZ_CURRENT:
            *value = eps_data_buff.minus_z_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_SP_PZ_CURRENT:
            *value = eps_data_buff.plus_z_solar_panel_ma;
            break;
        case EPS2_PARAM_ID_MPPT_1_DUTY_CYCLE:
            *value = eps_data_buff.mppt_1_duty_cycle;
            break;
        case EPS2_PARAM_ID_MPPT_2_DUTY_CYCLE:
            *value = eps_data_buff.mppt_2_duty_cycle;
            break;
        case EPS2_PARAM_ID_MPPT_3_DUTY_CYCLE:
            *value = eps_data_buff.mppt_3_duty_cycle;
            break;
        case EPS2_PARAM_ID_SP_VOLTAGE_MPPT:
            *value = eps_data_buff.total_solar_panels_mv;
            break;
        case EPS2_PARAM_ID_MAIN_POWER_BUS_VOLTAGE:
            *value = eps_data_buff.main_power_buss_mv;
            break;
        case EPS2_PARAM_ID_RTD_0_TEMP:
            *value = eps_data_buff.batteries_rtd0_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_1_TEMP:
            *value = eps_data_buff.batteries_rtd1_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_2_TEMP:
            *value = eps_data_buff.batteries_rtd2_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_3_TEMP:
            *value = eps_data_buff.batteries_rtd3_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_4_TEMP:
            *value = eps_data_buff.solar_panels_rtd4_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_5_TEMP:
            *value = eps_data_buff.solar_panels_rtd5_kelvin;
            break;
        case EPS2_PARAM_ID_RTD_6_TEMP:
            *value = eps_data_buff.solar_panels_rtd6_kelvin;
            break;
        case EPS2_PARAM_ID_BAT_VOLTAGE:
            *value = eps_data_buff.batteries_mv;
            break;
        case EPS2_PARAM_ID_BAT_CURRENT:
            *value = eps_data_buff.batteries_ma;
            break;
        case EPS2_PARAM_ID_BAT_AVERAGE_CURRENT:
            *value = eps_data_buff.batteries_average_ma;
            break;
        case EPS2_PARAM_ID_BAT_ACC_CURRENT:
            *value = eps_data_buff.batteries_accumulated_ma;
            break;
        case EPS2_PARAM_ID_BAT_CHARGE:
            *value = eps_data_buff.batteries_charge_mah;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_TEMP:
            *value = eps_data_buff.bm_temp_kelvin;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_STATUS:
            *value = eps_data_buff.bm_status_reg;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_PROTECT:
            *value = eps_data_buff.bm_protection_reg;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_CYCLE_COUNTER:
            *value = eps_data_buff.bm_cycle_counter_reg;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RAAC:
            *value = eps_data_buff.bm_raac_mah;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RSAC:
            *value = eps_data_buff.bm_rsac_mah;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RARC:
            *value = eps_data_buff.bm_rarc_percent;
            break;
        case EPS2_PARAM_ID_BAT_MONITOR_RSRC:
            *value = eps_data_buff.bm_rsrc_percent;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_1_DUTY_CYCLE:
            *value = eps_data_buff.heater1_duty_cycle;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_2_DUTY_CYCLE:
            *value = eps_data_buff.heater2_duty_cycle;
            break;
        case EPS2_PARAM_ID_FW_VERSION:
            *value = eps_data_buff.firmware_version;
            break;
        case EPS2_PARAM_ID_MPPT_1_MODE:
            *value = eps_data_buff.mppt_1_mode;
            break;
        case EPS2_PARAM_ID_MPPT_2_MODE:
            *value = eps_data_buff.mppt_2_mode;
            break;
        case EPS2_PARAM_ID_MPPT_3_MODE:
            *value = eps_data_buff.mppt_3_mode;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_1_MODE:
            *value = eps_data_buff.heater1_mode;
            break;
        case EPS2_PARAM_ID_BAT_HEATER_2_MODE:
            *value = eps_data_buff.heater2_mode;
            break;
        case EPS2_PARAM_ID_POWER_EN_REG:
            *value = eps_data_buff.power_en_reg;
            break;
        case EPS2_PARAM_ID_BUCK_EN_REG:
            *value = eps_data_buff.buck_en_reg;
            break;
        case EPS2_PARAM_ID_DEVICE_ID:
            *value = eps_data_buff.device_id;
            break;
        default:
            sys_log_print_event_from_module(SYS_LOG_ERROR, EPS_DATA_NAME, "Unknown parameter ID while reading!");
            sys_log_new_line();
			return -1;
    }
    return 0;
}

int eps_buffer_get_size(uint8_t id, uint8_t* size)
{
    if (size == 0) {
        sys_log_print_event_from_module(SYS_LOG_ERROR, EPS_DATA_NAME, "Null size pointer for getting size!");
        sys_log_new_line();
        return -1; // NULL pointer validation
    }

    switch(id)
    {
        /* --- 4 bytes parameters (uint32_t) --- */
        case EPS2_PARAM_ID_TIME_COUNTER:
        case EPS2_PARAM_ID_RTD_0_TEMP:
        case EPS2_PARAM_ID_RTD_1_TEMP:
        case EPS2_PARAM_ID_RTD_2_TEMP:
        case EPS2_PARAM_ID_RTD_3_TEMP:
        case EPS2_PARAM_ID_RTD_4_TEMP:
        case EPS2_PARAM_ID_RTD_5_TEMP:
        case EPS2_PARAM_ID_RTD_6_TEMP:
        case EPS2_PARAM_ID_FW_VERSION:
            *size = (uint8_t)sizeof(uint32_t);
            break;

        /* --- 2 bytes parameters (uint16_t) --- */
        case EPS2_PARAM_ID_MCU_TEMP:
        case EPS2_PARAM_ID_EPS_CURRENT:
        case EPS2_PARAM_ID_RESET_COUNTER:
        case EPS2_PARAM_ID_SP_MY_PX_VOLTAGE:
        case EPS2_PARAM_ID_SP_MX_PZ_VOLTAGE:
        case EPS2_PARAM_ID_SP_MZ_PY_VOLTAGE:
        case EPS2_PARAM_ID_SP_MY_CURRENT:
        case EPS2_PARAM_ID_SP_PY_CURRENT:
        case EPS2_PARAM_ID_SP_MX_CURRENT:
        case EPS2_PARAM_ID_SP_PX_CURRENT:
        case EPS2_PARAM_ID_SP_MZ_CURRENT:
        case EPS2_PARAM_ID_SP_PZ_CURRENT:
        case EPS2_PARAM_ID_SP_VOLTAGE_MPPT:
        case EPS2_PARAM_ID_MAIN_POWER_BUS_VOLTAGE:
        case EPS2_PARAM_ID_BAT_VOLTAGE:
        case EPS2_PARAM_ID_BAT_CURRENT:
        case EPS2_PARAM_ID_BAT_AVERAGE_CURRENT:
        case EPS2_PARAM_ID_BAT_ACC_CURRENT:
        case EPS2_PARAM_ID_BAT_CHARGE:
        case EPS2_PARAM_ID_BAT_MONITOR_TEMP:
        case EPS2_PARAM_ID_BAT_MONITOR_RAAC:
        case EPS2_PARAM_ID_BAT_MONITOR_RSAC:
        case EPS2_PARAM_ID_DEVICE_ID:
            *size = (uint8_t)sizeof(uint16_t);
            break;

        /* --- 1 byte parameters (uint8_t) --- */
        case EPS2_PARAM_ID_LAST_RESET_CAUSE:
        case EPS2_PARAM_ID_MPPT_1_DUTY_CYCLE:
        case EPS2_PARAM_ID_MPPT_2_DUTY_CYCLE:
        case EPS2_PARAM_ID_MPPT_3_DUTY_CYCLE:
        case EPS2_PARAM_ID_BAT_MONITOR_STATUS:
        case EPS2_PARAM_ID_BAT_MONITOR_PROTECT:
        case EPS2_PARAM_ID_BAT_MONITOR_CYCLE_COUNTER:
        case EPS2_PARAM_ID_BAT_MONITOR_RARC:
        case EPS2_PARAM_ID_BAT_MONITOR_RSRC:
        case EPS2_PARAM_ID_BAT_HEATER_1_DUTY_CYCLE:
        case EPS2_PARAM_ID_BAT_HEATER_2_DUTY_CYCLE:
        case EPS2_PARAM_ID_MPPT_1_MODE:
        case EPS2_PARAM_ID_MPPT_2_MODE:
        case EPS2_PARAM_ID_MPPT_3_MODE:
        case EPS2_PARAM_ID_BAT_HEATER_1_MODE:
        case EPS2_PARAM_ID_BAT_HEATER_2_MODE:
        case EPS2_PARAM_ID_POWER_EN_REG:
        case EPS2_PARAM_ID_BUCK_EN_REG:
            *size = (uint8_t)sizeof(uint8_t);
            break;

        default:
            sys_log_print_event_from_module(SYS_LOG_ERROR, EPS_DATA_NAME, "Unknown parameter ID while getting size!");
            sys_log_new_line();
            return -1;
    }

    return 0;
}

/** \} End of eps_data group */

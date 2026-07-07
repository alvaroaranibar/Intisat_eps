# 0. Power

## 0.1. Updates to initial design of power

The INTISAT EPS uses an unregulated main voltage bus, with the MPPT boost stage, the batteries and the main bus connected in parallel. Differentiated load switching is achieved through six buck regulators cascaded with power switches. The updated system block diagram is shown in Appendix A.

### 0.1.1. Power budget

_PENDING — to be drafted later (awaiting power budget figures / orbit scenario)._

### 0.1.2. External connectors

The EPS external electrical interfaces are shown in Fig. 1 and described in Table 1. A subset of them are **umbilical connectors**, routed from the EPS to a secondary board mounted parallel to one satellite face; they serve debug, testing and before-launch operations prior to deployer integration, and their availability depends on the deployer integration requirements. For example, some providers such as ISISPACE offer access ports to charge the battery close to T-0.

The **battery charger connector** (Table 1, No. 4) is the most critical umbilical interface: because the harvesting stage is an MPPT boost converter (§0.1.4), the main bus voltage (VBUS, imposed by the 2S2P pack) must stay above the post-deployment solar-panel voltage, so the pack must be pre-charged before launch to ensure correct boost operation and a positive initial energy balance.

The **PC/104 pinout** is shown in Fig. 2.

### 0.1.3. Solar pannels

Because the harvesting stage is an MPPT boost converter (§0.1.4), the array charges the 2S2P battery pack directly, dumping the solar power onto the main bus, which sits at 6.0 / 7.2 / 8.4 V (minimum / nominal / maximum). Each string is therefore limited to two series cells (2S), keeping its maximum-power voltage below the minimum pack voltage so the boost stage can always step it up to VBUS.

The baseline cell is the Spectrolab XTE-SF triple-junction CIC (32.2 % efficiency, 27 cm²), used as two cells in series per module. At 28 °C and AM0, the module parameters are:

$$V_{OC} = 2 \times 2.75\ \text{V} = 5.5\ \text{V}$$
$$V_{MP} = 2 \times 2.43\ \text{V} = 4.86\ \text{V}$$
$$I_{SC} = 27\ \text{cm}^2 \times 18.6\ \text{mA/cm}^2 = 502\ \text{mA}$$
$$I_{MP} = 27\ \text{cm}^2 \times 17.9\ \text{mA/cm}^2 = 483\ \text{mA}$$
$$P_{MP} = V_{MP} \times I_{MP} = 2.34\ \text{W}$$

Applying the −6 mV/°C per-cell temperature coefficient, at a representative 70 °C the maximum-power voltage and module power fall to:

$$V_{MP} = 2 \times [\,2.43 - 0.006 \times (70 - 28)\,]\ \text{V} = 4.35\ \text{V}$$
$$P_{MP} \approx 2.10\ \text{W}$$

Per-string current and voltage are sensed to feed the MPPT control loop; the complete measurement circuits are given in INTISAT_Power_FullSchematics__v1.A.pdf.

### 0.1.4. MPPT boost controller

The energy-harvesting stage uses three independent boost converters switching at 100 kHz, one per string, each operating as a Maximum Power Point Tracker between the solar panels and the main bus. One booster circuit is shown in Fig. 3. Each converter is driven by a Perturb & Observe (P&O) algorithm running on the MCU (see §0.3.3).

The architecture uses an unregulated bus, with the MPPT outputs, the batteries and the main bus tied in parallel and no dedicated Battery Charge Regulator. The battery current is therefore not regulated directly; it is the instantaneous surplus between the MPPT-extracted power and the load demand:

$$I_{bat} = \frac{\eta \cdot P_{MPPT}}{V_{bus}} - I_{loads}$$

where η is the conversion efficiency. The battery thus absorbs all system transients. When it reaches its upper voltage limit, the converter leaves peak-power tracking and enters a constant-voltage mode, adjusting the boost duty cycle to hold the bus voltage while the charge current decays naturally.

### 0.1.5. Power inhibit and activation interface

The power inhibit and activation interface isolates the EPS from its energy sources during integration and launch, as required for launch integration, and enables it after ejection from the deployer. It comprises one RBF connector and two 3-terminal kill micro-switches (Fig. 5); their combined activation logic is given in Table 2.

Current flow to the main bus is gated by four MOSFETs: two in parallel, for redundancy, connect the MPPT outputs to the main bus, and the other two, also redundant, connect the DS2777-protected battery voltage to the main bus. Fig. 4 shows one MOSFET pair; the complete circuit is given in INTISAT_Power_FullSchematics__v1.A.pdf.

### 0.1.6. Battery characterization and monitoring

The battery is a 2S2P pack of Sanyo NCR18650GA cells (Table 3), over a 6.0 / 7.2 / 8.4 V bus range. A DS2777 handles both protection and charge estimation. As a hardware protector, it acts autonomously within microseconds against short circuits, overcurrent and out-of-range voltages, controlling charge and discharge by driving the gates of back-to-back MOSFETs (see INTISAT_Power_FullSchematics__v1.A.pdf). As a fuel gauge (FuelPack algorithm), it estimates the remaining usable charge from a piecewise-linear model that depends on temperature and discharge rate.

The fuel gauge uses a 5 mΩ sense resistor, which measures up to 10.2 A, and is configured with an active-empty voltage of 3.2 V/cell (at 6 A pack, 3 A per branch), where RARC reaches 0 %, and a standby-empty voltage of 2.8 V/cell (at 1 A pack, 0.5 A per branch) as the model zero. A hardware undervoltage cutoff of 2.60 V/cell provides the last safety barrier before the 2.5 V cell limit.

Fig. 6 shows the three estimated FuelPack curves (Full, Active Empty and Standby Empty), which model, versus temperature, the maximum available capacity and the charge trapped under heavy (6 A) and light (1 A) loads. Lacking an environmental chamber, a hybrid model was used, combining the cell manufacturer datasheet with capacity data from the study *Evaluation of commercial Li-Ion 18650 battery cells for deep space applications* [ref], which evaluates the discharge of commercial 18650 cells at different temperatures. The +40 °C anchor is set to 7000 mAh.

This solution does not provide cell balancing and requires a complex per-pack characterization; future work plans a monitor IC that adds cell balancing and is characterization-free, such as one based on Maxim's modern ModelGauge m5 fuel-gauge algorithm.

### 0.1.7. Heaters and battery daughterboard

Two heaters keep the battery pack above its minimum operating temperature. Each is driven by a MOSFET driver (Fig. 7) from a 1 kHz PWM signal; the control strategy is described in §0.3.3.

The battery daughterboard (Fig. 8) hosts the four NCR18650GA cells (2S2P pack), the two heaters and four RTDs (RTD0, RTD1, RTD4 and RTD5) that sense the pack temperature and are read by the ADS1248. It connects to the EPS through the battery, RTD and heater interfaces (Table 1, No. 7, 8 and 9). The board also carries a PC/104 connector that is left unconnected and serves only to keep it stackable.

### 0.1.8. DC/DC power conversion and load switching

As shown in the updated block diagram (Appendix A), the DC/DC conversion and load-switching stage comprises six buck regulators: three 3.3 V/2 A, one 5 V/3 A and two 5 V/5 A. The regulators feed a set of power switches that provide differentiated load switching, and each buck is enabled automatically through GPIO only when at least one of its derived power switches is active, saving quiescent power; this management is detailed in §0.3.3. The detailed schematic is given in INTISAT_Power_FullSchematics__v1.A.pdf. The load assigned to each power switch is still to be defined.

Each buck requires a minimum bus (input) voltage to keep regulating, which sets the circuit cutoff voltage. For the low-dropout 5 V/5 A regulators (TPS54546B-Q1, D_max = 0.99) and the 5 V/3 A regulator (TPS5430-Q1, D_max = 0.87), it is:

$$V_{in,min} = \frac{V_{out} + V_F + R_{dc} \cdot I_{out}}{D_{max}} + R_{DS(on)} \cdot I_{out} - V_F$$

where V_F is the catch-diode forward voltage, R_dc the inductor DC resistance, R_DS(on) the switch on-resistance and I_out the load current. This yields:

$$V_{in,min}\,(5\,\text{V}/5\,\text{A}) = \frac{5 + 0.62 + 0.0155 \cdot 4}{0.99} + 0.092 \cdot 4 - 0.62 = 5.48\ \text{V}$$
$$V_{in,min}\,(5\,\text{V}/3\,\text{A}) = \frac{5 + 0.7 + 0.0518 \cdot 2.9}{0.87} + 0.100 \cdot 2.9 - 0.7 = 6.3\ \text{V}$$

The 3.3 V/2 A regulators (TPS5420-Q1) require 5.5 V.

### 0.1.9. Communication interfaces

Fig. 9 shows the EPS communication interfaces. In INTISAT's OBC-centric architecture the OBC is the system master, and the main link is an I2C bus (SDA/SCL) on which the EPS acts as a slave. The UART to the UHF transceiver (A0) is a redundant telemetry path, used only if the I2C link fails. The debug UART (A2, via a USB-TTL FT232 module) and the JTAG interface (via an MSP-FET430UIF) belong to the umbilical connectors.

Because the I2C link to the OBC runs between boards, it is decoupled through a TCA4311A hot-swap bus buffer (see INTISAT_Power_FullSchematics__v1.A.pdf) that isolates the bus capacitance of both segments, accelerates the edges over the inter-board harness, and supports live insertion. This mitigates I2C bus lock-ups, in which a stalled device holds the line low and hangs the entire bus, a recognized cause of in-orbit CubeSat failures.

## 0.2. Bill of materials status of power

The complete EPS bill of materials is given in Appendix B. Most components were selected as automotive-grade or with flight heritage. Only one part remains to be procured: a 4-pin Molex Pico-Lock connector for the unregulated supply to the payload controller.

## 0.3. Power integration

### 0.3.1. PCB fabrication and assembly

The EPS was assembled in-house, following three steps for the double-sided SMD assembly.

**a) Solder paste application.** Solder paste was applied through a manual stencil printer (Fig. 10), using a separate stencil per side: SAC305 (217 °C) on the top side and Sn63Pb37 (183 °C) on the bottom side.

**b) Component placement.** SMD components were placed on the pasted pads with the manual Bungard SMT 3000 pick-and-place machine (Fig. 11).

**c) Reflow soldering.** Each side was reflowed in the Quartz Reflow Oven Hot Air 3000 following the profiles in Table 4 (Fig. 12 and Fig. 13), which comply with the IPC/JEDEC J-STD-020 classification reflow profile (Appendix D): ramp-up below 3 °C/s, ramp-down below 6 °C/s, and a bounded time above liquidus. Because most EPS pads lack thermal reliefs, which increases the tombstoning risk, an extended preheat/soak stage is used to homogenize the board temperature and avoid thermal gradients. The top side was reflowed first, with SAC305 peaking at 245 °C, and the bottom side second, with Sn63Pb37 peaking at 220 °C; since the second reflow stays below the 217 °C top-side melting point, those joints do not remelt and their components do not fall off. For the second reflow the board was raised on hexagonal standoffs to keep the top-side components clear of the base. Finally, all through-hole components were hand-soldered, and every joint was inspected under a microscope, correcting any defect by hand.

### 0.3.2. Power development cost

_PENDING — to be drafted later (awaiting cost figures)._

### 0.3.3. Firmware

The EPS firmware runs on a Texas Instruments MSP430F6659 microcontroller. The main and subsystem clocks (MCLK and SMCLK) run at 32 MHz and the auxiliary clock (ACLK) at 32.768 kHz. The firmware is built on the MSP430 DriverLib, used as the hardware abstraction layer over the microcontroller registers.

**a) Drivers and devices**

Above the HAL, the firmware is organized in two layers. The driver layer provides low-level access to the microcontroller peripherals and external ICs, including GPIO, ADC, SPI, I2C (master and slave), UART, PWM, Flash and the watchdog timer, plus device-specific drivers for the ADS1248 RTD ADC, the DS277Xg fuel gauge and the TCA4311A I2C buffer. The device layer builds on these drivers to expose high-level abstractions used by the application tasks, such as the current, voltage and temperature sensors, and the MPPT, heater, battery monitor, power switch, OBDH and TTC devices.

**b) Operative system**

FreeRTOS is used as the real-time operating system to guarantee deterministic scheduling of the periodic tasks. Its main configuration is summarized in Table 5.

**Table 5. FreeRTOS main configuration**

| Parameter | Value |
| --- | --- |
| Kernel version | v10.2.1 |
| Scheduling | Preemptive |
| Tick rate | 1000 Hz |
| Max priorities | 5 |
| Total heap size | 40 KB |
| Stack overflow check | Enabled (method 2) |

**c) Tasks**

The firmware is structured as a set of periodic and event-driven tasks, listed with their scheduling parameters and function in Table 6. Priorities range from 1 (lowest) to 5 (highest).

**Table 6. Firmware tasks**

| Task | Priority | Period | Stack (bytes) | Function |
| --- | --- | --- | --- | --- |
| Startup | 5 | aperiodic | 500 | Initializes the clocks, logger and all devices/drivers, then unblocks the remaining tasks and suspends itself. |
| Parameter Server | 4 | on interrupt | 300 | Serves OBC and TTC telemetry read/write commands and the safe-mode event; woken by ISR notifications. |
| Heater Controller | 3 | 2000 ms | 160 | Controls the two battery heaters in automatic or manual mode. |
| MPPT | 3 | 300 ms | 160 | Runs the P&O algorithm on the three boost channels, or applies a commanded duty cycle in manual mode. |
| Time Control | 3 | 1000 ms | 128 | Maintains the system time and periodically stores it in non-volatile memory. |
| Read Sensors | 2 | 60000 ms | 512 | Reads all sensors into the telemetry buffer (panels, bus, battery monitor, RTDs, MCU). |
| System Reset | 2 | 10 h | 128 | Performs a periodic software reset to clear latent state. |
| Watchdog Reset | 1 | 100 ms | 150 | Refreshes the internal watchdog timer. |
| Heartbeat | 1 | 500 ms | 160 | Blinks the status LED at ~1 Hz (engineering model only). |

**Task synchronization.** Three FreeRTOS mechanisms coordinate the tasks. First, an event group (`task_startup_status`) gates execution: every task blocks on it until the Startup task signals completion, so no task accesses a device before initialization has finished. Second, task notifications act as lightweight signals from the interrupt service routines to the Parameter Server, which blocks waiting for them; each source (OBC I2C, TTC UART and the safe-mode interrupt) sets a distinct notification bit, as detailed in the next subsection. Third, mutexes protect the resources shared between tasks, namely the ADC (accessed by the Read Sensors and MPPT tasks through the current, voltage and temperature sensors) and the debug-log UART.

## 0.4. Power operation and tests

### 0.4.1. Electrical tests

_TBD._

### 0.4.2. Firmware tests

_TBD._

### 0.4.3. Thermal tests

_TBD._

---

# Power appendices

## Appendix A: Updated block diagram

_TBD._

## Appendix B: Bill of materials (BOM)

_TBD._

## Appendix C: I2C/UART telemetry protocols logic analyzer captures

_TBD._

## Appendix D: Reflow profile per IPC/JEDEC J-STD-020

_TBD._

# GSM-Based Plant Monitoring and Pump Control System Using LPC2129

A GSM-enabled embedded system that allows a user to check soil condition
and operate a water pump remotely through SMS. The project is
implemented using the **LPC2129 ARM7 microcontroller**, **SIM800C GSM
module**, **16×2 LCD**, digital soil-moisture sensor, and a motor-driver
interface.

## Table of Contents

-   [Overview](#overview)
-   [Objectives](#objectives)
-   [Hardware Components](#hardware-components)
-   [Software and Tools](#software-and-tools)
-   [System Block Diagram](#system-block-diagram)
-   [Software Architecture](#software-architecture)
-   [Working Principle](#working-principle)
-   [SMS Commands](#sms-commands)
-   [Peripheral Interfacing](#peripheral-interfacing)
-   [Learning Outcomes](#learning-outcomes)
-   [Important Implementation Notes](#important-implementation-notes)
-   [Future Enhancements](#future-enhancements)

## Overview

This project demonstrates remote plant monitoring and pump control using
SMS communication. The SIM800C GSM module receives SMS messages and
communicates with the LPC2129 through UART0. The microcontroller
extracts the command and performs the requested operation.

The uploaded firmware currently supports two commands:

-   `*status#` --- reads the digital soil-moisture input and replies
    `DRY` or `WET`.
-   `*pump on#` --- switches the motor-control output ON for 60 seconds,
    then switches it OFF and sends status messages.

A 16×2 LCD displays the system and pump status. The soil sensor is read
as a digital input.

## Objectives

-   Implement SMS-based communication using the SIM800C GSM module.
-   Interface the SIM800C with the LPC2129 using UART0.
-   Receive and parse user commands from SMS.
-   Monitor soil condition using a digital soil-moisture sensor.
-   Control a pump/motor through an external driver or relay interface.
-   Display system messages and pump status on a 16×2 LCD.
-   Organize the firmware into reusable GSM, UART, LCD, motor, and delay
    modules.

## Hardware Components

-   LPC2129 ARM7 microcontroller
-   SIM800C GSM module with SIM card and antenna
-   16×2 character LCD
-   Digital soil-moisture sensor module
-   DC motor / water pump
-   Suitable motor driver or relay-driver circuit
-   Regulated power supply for the LPC2129 and LCD
-   Stable power supply suitable for SIM800C current bursts
-   Separate, correctly rated motor supply
-   Connecting wires and supporting components

## Software and Tools

-   Embedded C
-   Keil µVision IDE
-   LPC2129 device header (`lpc21xx.h`)
-   UART serial terminal for GSM testing
-   SIM800C AT command interface
-   Proteus or equivalent simulation tool (optional, if used)

## System Block Diagram

<img width="1774" height="887" alt="GSM Plant Monitoring and Pump Control Diagram" src="https://github.com/user-attachments/assets/a3e93851-5327-4e50-97e2-fc3a13c01de7" />


**Signal summary:** The SIM800C and LPC2129 communicate through UART0;
the soil sensor provides a digital input; the LCD receives 4-bit data
and control signals; and P0.8/P0.9 control an external motor-driver
stage.

## Software Architecture

### 1. Main Application --- `main.c`

Responsible for: - Configuring the soil-sensor pin as input. -
Configuring the motor-control pins as outputs. - Setting the motor
outputs OFF during startup. - Initializing UART0, GSM, and LCD
modules. - Receiving the SMS command into `received_msg`. - Comparing
the received command with `status` and `pump on`. - Reading soil
condition and sending the corresponding reply. - Running the pump for 60
seconds for the `pump on` command, then switching it OFF.

### 2. GSM Driver --- `gsm_header.h`

Provides: - `GSM_CONFIG()` --- sends modem initialization AT commands. -
`GSM_SEND_MSG(char *msg)` --- sends an SMS using `AT+CMGS` and Ctrl+Z
(`0x1A`). - `GSM_RECEIVE_SMS(char *buffer)` --- stores received
characters between the `*` and `#` delimiters.

The configured SMS recipient is defined by `USER_NUMBER`. Replace the
example number in your local source with the intended recipient number,
and avoid publishing personal phone numbers in a public repository.

### 3. UART Driver --- `uart_header.h`

Provides: - `UART0_CONFIG()` --- configures UART0. -
`UART0_TX(unsigned char)` --- transmits one character. - `UART0_RX()`
--- receives one character. - `UART0_STR(char *)` --- transmits a
string.

### 4. LCD Driver --- `lcdheader.h`

Provides: - `LCD_INITIALIZATION()` - `LCD_COMMAND(unsigned char)` -
`LCD_DATA(unsigned char)` - `LCD_STR(unsigned char *)`

The LCD is configured in 4-bit mode.

### 5. Motor Driver --- `motor.c` and `motor.h`

Provides: - `MOTOR_ON()` --- sets the P0.8 control output and clears
P0.9. - `MOTOR_OFF()` --- clears both P0.8 and P0.9.

These GPIO signals must connect to a suitable external driver/relay
interface, not directly to the motor.

### 6. Delay Module --- `delayheader.h`

Provides: - `delay_seconds(unsigned int)` -
`delay_milliseconds(unsigned int)`

The delay functions use Timer0 registers. Their timing depends on the
configured peripheral clock.

## Working Principle

1.  The LPC2129 configures P0.2 as the soil-sensor input and P0.8/P0.9
    as motor-control outputs.
2.  The motor outputs are cleared so the motor starts in the OFF state.
3.  UART0 is initialized for communication with the SIM800C.
4.  The GSM configuration routine sends `AT`, selects SMS text mode
    using `AT+CMGF=1`, and configures new-message indications using
    `AT+CNMI=2,2,0,0,0`.
5.  The LCD is initialized and displays `SMART IRRIGATION`.
6.  The controller waits for incoming UART data. The current receive
    function waits for `*`, stores characters until `#`, and terminates
    the command string.
7.  If the command is `status`, the controller reads P0.2:
    -   P0.2 HIGH: the code treats the soil as dry, displays
        `SOIL IS DRY`, and sends `DRY`.
    -   P0.2 LOW: the code treats the soil as wet, displays
        `SOIL IS WET`, and sends `WET`.
8.  If the command is `pump on`, the controller calls `MOTOR_ON()`,
    displays `PUMP ON`, and sends a `PUMP ON` SMS.
9.  The firmware waits 60 seconds, calls `MOTOR_OFF()`, displays
    `PUMP OFF`, and sends a `PUMP OFF` SMS.
10. The main loop repeats and waits for another command.

## SMS Commands

Send the SMS body with the delimiters shown below. The receive routine
removes the delimiters and compares the text inside them.

  SMS body      Command stored by firmware   Operation
  ------------- ---------------------------- ----------------------------------------
  `*status#`    `status`                     Read soil sensor; reply `DRY` or `WET`
  `*pump on#`   `pump on`                    Run motor for 60 seconds, then stop

**Command matching is case-sensitive.** Use lowercase text exactly as
shown. The uploaded code does not currently implement a separate
`pump off` command or automatic pump control based on soil moisture.

## Peripheral Interfacing

### UART0 --- LPC2129 to SIM800C

The source configures UART0 using `PINSEL0 = 0x05`.

  LPC2129 signal   SIM800C signal   Purpose
  ---------------- ---------------- ---------------------------
  P0.0 / TXD0      RXD              MCU transmits AT commands
  P0.1 / RXD0      TXD              MCU receives modem data
  GND              GND              Common signal reference

Confirm logic-level compatibility and provide the SIM800C with a stable
supply capable of its transmit-current peaks.

### LCD --- 4-bit interface

The source defines `LCD_D` as bits 14--17 and uses P0.12 and P0.13 for
RS and Enable.

  LCD signal   LPC2129 pin
  ------------ ------------------------
  D4           P0.14
  D5           P0.15
  D6           P0.16
  D7           P0.17
  RS           P0.12
  E            P0.13
  R/W          GND for write-only use

Connect LCD VSS, VDD, and contrast pin according to the LCD module
requirements.

### Soil Sensor

  Signal           LPC2129 pin
  ---------------- -------------
  Digital output   P0.2

The source treats LOW as wet and HIGH as dry. Confirm the sensor
module's output polarity before using the system.

### Motor Control

  LPC2129 pin   Firmware definition   Function
  ------------- --------------------- ------------------------
  P0.8          `MOTOR_IN`            Motor control output 1
  P0.9          `MOTOR_OUT`           Motor control output 2

Use a correctly rated motor driver or relay circuit with appropriate
protection. Never connect the motor directly to the LPC2129 GPIO pins.

## Learning Outcomes

-   LPC2129 ARM7 microcontroller programming
-   UART0 serial communication
-   GSM modem control using AT commands
-   SMS transmission and reception
-   C string buffering and command comparison
-   16×2 LCD interfacing in 4-bit mode
-   Digital sensor interfacing
-   GPIO-based motor control
-   Modular Embedded C development
-   Timer0-based delay generation

## Important Implementation Notes

-   The SMS receive function in the uploaded code expects `*` and `#`
    delimiters in the received UART stream. The SMS body should
    therefore be sent in that format, and the actual SIM800C UART
    response format must be checked during integration.
-   `AT+CNMI=2,2,0,0,0` can deliver a modem notification/header followed
    by SMS text. The current `GSM_RECEIVE_SMS()` routine does not parse
    the full `+CMT` response structure; ensure the delimiters are
    present in the body and that the receive routine is synchronized
    correctly.
-   The receive function blocks while waiting for `*` and `#`. A more
    robust implementation should include timeouts, UART error handling,
    and buffer-overflow handling.
-   The receive buffer stores up to 99 message characters.
-   Verify the UART baud rate and Timer0 timing against the actual
    peripheral clock configuration.
-   The source includes `Motor.c` in `main.c`. In a conventional
    multi-file Keil project, include `motor.h` in `main.c` and add
    `motor.c` as a separate source file to the project; do not compile
    the same implementation twice.
-   Confirm all pin assignments against the physical circuit before
    powering the hardware.

## Safety

This is an educational prototype, not a certified irrigation or
industrial control system.

-   Never drive a motor or relay coil directly from an LPC2129 GPIO.
-   Use a suitable motor driver or correctly rated relay/contactor,
    protection components, and a separate motor supply.
-   Keep motor and mains wiring isolated from low-voltage MCU/GSM
    circuitry.
-   Test GPIO and driver operation with motor power disconnected first.
-   Ensure the motor defaults to OFF after reset or power failure.
-   Do not leave a pump unattended without suitable water-level,
    overload, and dry-run protection.

## Future Enhancements

-   Implement a dedicated `pump off` SMS command.
-   Add authorized-sender validation before accepting pump commands.
-   Add robust SMS parsing and UART receive timeouts.
-   Make pump run time configurable.
-   Add water-level monitoring and dry-run protection.
-   Implement automatic irrigation using the soil-moisture reading.
-   Add periodic sensor reports and fault alerts by SMS.

## Repository Structure

A suggested repository layout is:

``` text
GSM-based-Plant-monitoring-and-Pump-control-system-using-LPC2129/
├── README.md
├── main.c
├── gsm_header.h
├── uart_header.h
├── lcdheader.h
├── motor.c
├── motor.h
└── delayheader.h
```

## Project Information

-   **Project Title:** GSM-Based Plant Monitoring and Pump Control
    System Using LPC2129
-   **Microcontroller:** LPC2129 (ARM7)
-   **GSM Module:** SIM800C
-   **Display:** 16×2 LCD
-   **Sensor:** Digital soil-moisture sensor
-   **Communication:** UART0 and GSM SMS
-   **Programming Language:** Embedded C
-   **IDE:** Keil µVision



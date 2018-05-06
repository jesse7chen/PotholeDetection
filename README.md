# Autonomous Pothole Detection and Cataloging for Bikes

- Pothole detection system utilizing accelerometer, computer vision, and manual reporting techniques.

- Stores up to 240 potholes in local flash memory and warns user if approaching a known pothole.

- Outputs pothole locations via BLE in order to simulate upload to remote cloud server.

- Built on LPC1114 (ARM Cortex M0) with a BeagleBone Black used for computer vision processing.

- Built with Keil uVision IDE

See final report PDF for more details

## Overview
### Block Diagram
![Block Diagram](https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Final_445BlockDiagram.png)

The entire system is powered by a commercial battery pack with 10000 mAh capacity. There are two voltage regulators: the 3.3V regulator supplies power to the main MCU and the 5V regulator supplies power to the BeagleBone Black.

The system can detect potholes via accelerometer, computer vision, or manual reporting. Computer vision is implemented on the BeagleBone Black, which notifies the main MCU via GPIO pin interrupt when it detects a pothole. The other methods are directly connected to the MCU.

Upon detection of a pothole, the system takes its latest GPS coordinate (if valid) and outputs it to the bluetooth module, which should be connected to the user's phone. This is to simulate upload to a cloud database, which we were not able to complete during the project. It also pushes the new pothole location into the MCU's on-chip flash memory.

Every time we receive a new GPS location, we check it against our local database of known potholes. If we detect that we are near a pothole and are heading towards it, we alert the user via a vibrating buzzer which is wrapped around the user's arm.

## Methods of Detection
### Accelerometer
Module: MMA7660FCT
The accelerometer was used to detect potholes that bikers had already ridden over. We communicated with it over I2C at 100 kHz and sampled from it at 120 Hz using a software timer.

The algorithm utilized to detect potholes was the simple z-differential algorithm, which takes the difference between the current and previous accelerometer values to compare to a threshold. If the difference is greater than the threshold, we say that there is a pothole.

With a threshold of 1.9 Gs, we were able to detect major potholes and had nearly no false positives (>95% accuracy).

### Computer Vision

### Manual Reporting
In the case where the user spotted the pothole and computer vision did not, we allow the user to manually report the pothole using a button mounted on the handlebar of the bike.

A software debouncer is utilized to minimize PCB space. It simply polls the button every 5ms and looks for ~12 low reads in a row before notifying the system that the button has been pressed.

## Cataloging and Warning
### GPS
Module: FGPMMOPA6H (MTK3339-based)
The GPS communicates via UART, so we implemented an interrupt-based UART driver for the MCU. The module outputs ASCII strings following NMEA protocol. The software flow goes like so:
- At bootup: Tell GPS to output only RMC-type messages and change baudrate from 9600 to 115200 to decrease wait time.
- Every UART interrupt: Use state machine to keep track of status of message buildup (searching for start character, searching for end characters)
- Upon finding end characters: Tell system there is a valid message to be parsed. Check data validity field and verify checksum.
- If previous tasks pass, parse message into static structure.

## Bluetooth



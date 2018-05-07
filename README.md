# Autonomous Pothole Detection and Cataloging for Bikes

- Pothole detection system utilizing accelerometer, computer vision, and manual reporting techniques.

- Stores up to 240 potholes in local flash memory (4kB) and warns user if approaching a known pothole.

- Outputs pothole locations via BLE in order to simulate upload to remote cloud server.

- Built on LPC1114 (ARM Cortex M0) with a BeagleBone Black used for computer vision processing.

- Built with Keil uVision IDE

See final report PDF for more details

## Overview
### Block Diagram
![Block Diagram](https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Final_445BlockDiagram.png)

The entire system is powered by a commercial battery pack with 10000 mAh capacity. There are two voltage regulators: the 3.3V regulator supplies power to the main MCU and the 5V regulator supplies power to the BeagleBone Black.

The system can detect potholes via accelerometer, computer vision, or manual reporting. Computer vision is implemented on the BeagleBone Black, which notifies the main MCU via GPIO pin interrupt when it detects a pothole. The other methods are directly connected to the MCU.

All of the detection method flags are set in interrupt handlers, so we handle them upon return to the main loop. Upon detection of a pothole, the system takes its latest GPS coordinate (if valid) and outputs it to the bluetooth module, which should be connected to the user's phone. This is to simulate upload to a cloud database, which we were not able to complete during the project. It also pushes the new pothole location into the MCU's on-chip flash memory.

Every time we receive a new GPS location, we check it against our local database of known potholes. If we detect that we are near a pothole and are heading towards it, we alert the user via a vibrating buzzer which is wrapped around the user's arm. In addition, if the computer vision module detects a pothole, we also alert the user in case they did not see the pothole.

In order to avoid overcrowding the database, we set a flag upon detecting a pothole which prevents us from reporting another pothole until the user has moved at least 10 m away from the previous pothole. 

### System Photos
Top view

<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Top%20System.jpg" width="480">

Side view

<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Handle%20Bar%20system2.jpg" width="480">

As you can see, the system is meant to be mounted on the bikes handlebars.

### Main Demo Videos
Accererometer Detection + Nearby Pothole Alert Demo

[![Demo Video](https://img.youtube.com/vi/r6DLgzwXEwc/0.jpg)](https://youtu.be/r6DLgzwXEwc)

Nearby Pothole Alert Demo

[![Demo Video](https://img.youtube.com/vi/jkjBhqxgffA/0.jpg)](https://youtu.be/jkjBhqxgffA)

Accelerometer Demo (look at bottom of phone screen to see that the pothole was detected)

[![Demo Video](https://img.youtube.com/vi/9h0uMwb1o3E/0.jpg)](https://youtu.be/9h0uMwb1o3E)

## Methods of Detection
### Accelerometer
Module: MMA7660FCT

The accelerometer was used to detect potholes that bikers had already ridden over. We communicated with it over I2C at 100 kHz and sampled from it at 120 Hz using a software timer.

The algorithm utilized to detect potholes was the simple z-differential algorithm, which takes the difference between the current and previous accelerometer values to compare to a threshold. If the difference is greater than the threshold, we say that there is a pothole.

With a threshold of 1.9 Gs, we were able to detect major potholes and had nearly no false positives (>95% accuracy).

### Computer Vision
See final report PDF.

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

### Bluetooth
Module: Bluefruit LE SPI Friend

The bluetooth simulated upload to a cloud database and also provided a convenient method of debugging. A basic SPI driver was written for it, which was wrapped by a higher level SDEP driver. There are a few quirks required to work with this module including:
- 100us delay had to be added between asserting the chip select (CS) line and writing any data on the SPI bus
- Rather than toggling the CS line every byte, the CS line had to be asserted for the entirely of a packet, which could be up to 20 bytes
- Seemingly random 4ms delay had to be added between each packet (though this was probably to give time for the module to send data back to the MCU)

### Database
The database stores all our known potholes and searches for potholes that the user is in danger of hitting.

The search feature works in two stages:

**Stage 1** (if warningStatus set to 0)
1. Grab user's location
2. Compare user's location with pothole locations in database
3. If locations are within 25 m of each other, set warningStatus = 1 and store pothole in list of risky potholes
4. Keep searching until we have three risky potholes or database has been exhausted

**Stage 2** (if warningStatus set to 1)
1. Grab user's location
2. Compare location with pothole locations in list of risky potholes to see if we are still in danger.
3. If we are still within 25m, compare user's current bearing with the expected bearing if the user was headed towards the pothole.
4. If they are within 50 degrees of each other, alert the user via vibrating buzzer.
5. Set warningStatus back to 0, exit.

Equirectangular approximation is used to calculate distances between GPS coordinates, as it was nearly 3x faster than other methods (Haversine formula, Law of Cosines formula).

Since we are using NAND flash memory, we can only write 1's to 0's. To write 0's to 1's requires setting all 4kB of our database to 1's (which is called an erase). In addition, we can only write in 256 byte chunks, on 256 byte word boudnaries.

Therefore, we utilize the first 256 bytes of our database to store the current index in the database we are at. This segment is initially set to all 0xFF's, which are replaced by 0x00's as we add potholes. At bootup, we count the number of 0x00 bytes in this section to determine our current index. This could be made more efficient by setting individual bits to 0, but this is unnecessary as our database cannot store that many locations anyway.

When we store new potholes in our database, we must take care to pad it with previous pothole data and 0xFF's to avoid modifying any unnecessary bits.

## Physical Design
### Schematic
<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Control%20Module.PNG" width="960">

### PCB
<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/PCB.PNG" width="700">

### 3D Printed Mount
Main mount

<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/3dmodelbody.PNG" width="960">

Camera mount

<img src="https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/3dcam.PNG" width = "480">

## Additional Demo Videos
Full playlist of demos can be found [here](https://www.youtube.com/playlist?list=PL9imQhiwCe0fOyAjjxw4vcOPIk4TiATaH)

Demonstrating that database remains after power cycle

[![Demo Video](https://img.youtube.com/vi/pUO2MMN-PtQ/0.jpg)](https://youtu.be/pUO2MMN-PtQ)

Additional nearby pothole alert demo

[![Demo Video](https://img.youtube.com/vi/3CjQbx19XfQ/0.jpg)](https://youtu.be/3CjQbx19XfQ)

Extended Accelerometer Demo

[![Demo Video](https://img.youtube.com/vi/ZZC9G7tS7Gs/0.jpg)](https://youtu.be/ZZC9G7tS7Gs)

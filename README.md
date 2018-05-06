# Autonomous Pothole Detection and Cataloging for Bikes

- Pothole detection system utilizing accelerometer, computer vision, and manual reporting techniques.

- Stores up to 240 potholes in local flash memory and warns user if approaching a known pothole.

- Outputs pothole locations via BLE in order to simulate upload to remote cloud server.

- Built on LPC1114 (ARM Cortex M0) with a BeagleBone Black used for computer vision processing.

- Built with Keil uVision IDE

See wiki for more details

## Overview
### Block Diagram
![Block Diagram](https://github.com/jesse7chen/PotholeDetection/blob/master/imgs/Final_445BlockDiagram.png)

## Methods of Detection
### Accelerometer



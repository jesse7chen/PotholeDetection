#ifndef _CAMERA_DETECT_H_
#define _CAMERA_DETECT_H_

#include <stdint.h>

void cameraDetectInit(void);

uint8_t getCameraDetect(void);

void setCameraDetect(uint8_t n);

#endif /* _CAMERA_DETECT_H_ */

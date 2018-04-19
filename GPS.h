#ifndef _GPS_H_
#define _GPS_H_

#include <stdint.h>


typedef enum locationStatus {valid, invalid, stale} locationStatus;

typedef struct location_t 
{
    double latitude;
    double longitude;
    double speed;
    double bearing;
    locationStatus status;
} location_t;

// Main functions
int GPS_init(void);

void readGPS(void);

// Getters and setters
void resetGPSstatus(void);

unsigned int getGPSstatus(void);

unsigned int getGPSreadSuccess(void);

void resetGPSreadSuccess(void);

location_t getCurrLocation(void);

// Test/debugging functions
void testNMEA(void);

void printLocation(void);

void blePrintBuffer(void);

// Static helper functions
static double simple_strtod(const char* str);

static int readNMEA(void);

static int calcBufferChecksum(void);

static int verifyChecksum(void);

static int parseNMEA(void);

static int parseRMC(void);

static double dec_minutes_to_dec_degrees(char* dec_minutes, int isLat);

static double knots_to_mph(double knots);


#endif /* _GPS_H_ */


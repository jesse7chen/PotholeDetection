#ifndef _GPS_H_
#define _GPS_H_

#include <stdint.h>


typedef enum locationStatus {valid, invalid, stale} locationStatus;

typedef struct location_t 
{
    double latitude;
    double longitude;
    double speed;
    locationStatus status;
} location_t;


int GPS_init(void);

void readGPS(void);

void resetGPSstatus(void);

unsigned int getGPSstatus(void);

location_t getCurrLocation(void);

void testNMEA(void);

void printLocation(void);

static double simple_strtod(const char* str);

static int readNMEA(void);

static int calcBufferChecksum(void);

static int verifyChecksum(void);

static int parseNMEA(void);

static int parseRMC(void);

static double dec_minutes_to_dec_degrees(char* dec_minutes, int isLat);

static double knots_to_mph(double knots);


#endif /* _GPS_H_ */


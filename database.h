#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "GPS.h"

double distBetweenLocs(location_t loc1, location_t loc2);

static double degToRads(double degrees);

static uint8_t searchDatabase(location_t currLoc);

#endif /* _DATABASE_H_ */

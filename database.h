#ifndef _DATABASE_H_
#define _DATABASE_H_

#include "GPS.h"

typedef struct database_loc_t 
{
    double latitude;
    double longitude;
} database_loc_t;

int databaseInit(void);

uint8_t searchDatabase(location_t currLoc);

int insertLocation(location_t loc);

double distBetweenLocs(database_loc_t loc1, database_loc_t loc2);

double bearingBetweenLocs(database_loc_t loc1, database_loc_t loc2);

database_loc_t retrieveLocation(unsigned int idx);

static double degToRads(double degrees);

static unsigned int findDatabaseIdx(void);




#endif /* _DATABASE_H_ */
